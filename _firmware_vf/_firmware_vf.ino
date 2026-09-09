#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <math.h>
#include "driver/timer.h"
#include "esp_pm.h"
#include "esp32/pm.h"
#include "soc/rtc.h"

#define PIN_TURBIDITY     36
#define PIN_DEPTH         39
#define PIN_LED_LOW_FREQ  4
#define PIN_LED_HIGH_FREQ 2
#define PIN_LED_TX        15

#define OLED_SDA          33
#define OLED_SCL          32

// R-2R ladder 12-bit pin mapping
// D0 se D11 tak, LSB se MSB
const uint8_t dacPins[12] = {
    13, 12, 14, 27, 26, 25,  // D0-D5
    35, 34, 17, 16, 5, 18    // D6-D11
};

#define PI 3.14159265359
#define SAMPLE_RATE 500000
#define OLED_I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DMA_BUFFER_SIZE 2048
#define TRANSMIT_INTERVAL 1000

enum WindowType {
    WINDOW_HAMMING,
    WINDOW_HANN,
    WINDOW_BLACKMAN
};

enum WaveformType {
    CW_PULSE,
    LFM_UP_CHIRP,
    LFM_DOWN_CHIRP,
    PHASE_CODED,
    GEOMETRIC_SWEEP
};

struct EnvironmentData {
    float turbidityPercent;
    float depthPercent;
};

struct SonarParameters {
    float centerFrequency;
    float bandwidth;
    float pulseDuration;
    uint16_t amplitude;
    WaveformType type;
    WindowType window;
    String typeName;
    String windowName;
    String environmentMode;
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

EnvironmentData envData;
SonarParameters currentParams;
SonarParameters previousParams;

uint16_t dmaBuffer1[DMA_BUFFER_SIZE];
uint16_t* currentBuffer = dmaBuffer1;
volatile bool bufferReady = false;
volatile bool transmitting = false;

hw_timer_t* sampleTimer = NULL;
volatile uint32_t sampleIndex = 0;
volatile uint32_t totalSamples = 0;

unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastTransmission = 0;

void IRAM_ATTR onSampleTimer();
void IRAM_ATTR setR2RDACFast(uint16_t value);

void setup() {
    Serial.begin(115200);
    delay(100);
    
    // I2C bus initialize for OLED
    Wire.begin(OLED_SDA, OLED_SCL);
    Wire.setClock(400000);
    
    // R-2R ladder GPIO setup
    for(int i = 0; i < 12; i++) {
        pinMode(dacPins[i], OUTPUT);
        digitalWrite(dacPins[i], LOW);
    }
    
    // OLED display initialization
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS)) {
        Serial.println("OLED not found");
    } else {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(20, 20);
        display.println("S.A.L.S.A");
        display.display();
        delay(2000);
    }
    
    // LED pins configuration
    pinMode(PIN_LED_LOW_FREQ, OUTPUT);
    pinMode(PIN_LED_HIGH_FREQ, OUTPUT);
    pinMode(PIN_LED_TX, OUTPUT);
    
    // ADC for potentiometers
    analogReadResolution(12);
    analogSetPinAttenuation(PIN_TURBIDITY, ADC_11db);
    analogSetPinAttenuation(PIN_DEPTH, ADC_11db);
    
    // Hardware timer at 500 kHz sampling frequency
    sampleTimer = timerBegin(500000);
    timerAttachInterrupt(sampleTimer, &onSampleTimer);
    timerStart(sampleTimer);
    
    // Dynamic power management configuration
    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 80,
        .light_sleep_enable = true
    };
    esp_pm_configure(&pm_config);
    
    // Initial sonar parameters
    currentParams.centerFrequency = 250000;
    currentParams.bandwidth = 50000;
    currentParams.pulseDuration = 0.010;
    currentParams.amplitude = 2048;
    currentParams.type = LFM_UP_CHIRP;
    currentParams.window = WINDOW_HAMMING;
    currentParams.typeName = "LFM Up";
    currentParams.windowName = "Hamming";
    currentParams.environmentMode = "Initializing";
    
    Serial.println("S.A.L.S.A Autonomous Mode Started");
}

void IRAM_ATTR onSampleTimer() {
    if (transmitting && sampleIndex < totalSamples) {
        setR2RDACFast(currentBuffer[sampleIndex]);
        sampleIndex++;
        
        // Visual feedback during transmission
        if (sampleIndex % 200 == 0) {
            digitalWrite(PIN_LED_TX, !digitalRead(PIN_LED_TX));
        }
    } else if (sampleIndex >= totalSamples) {
        transmitting = false;
        sampleIndex = 0;
        setR2RDACFast(0);
        digitalWrite(PIN_LED_TX, LOW);
    }
}

void IRAM_ATTR setR2RDACFast(uint16_t value) {
    // Write 12-bit value to R-2R ladder pins
    for(int i = 0; i < 12; i++) {
        digitalWrite(dacPins[i], (value >> i) & 1);
    }
}

void loop() {
    // Environmental sensing every 150ms
    if (millis() - lastSensorRead > 150) {
        readEnvironment();
        adaptParametersAuto();
        applyPowerSaving();
        lastSensorRead = millis();
    }
    
    // Autonomous transmission every second
    if (!transmitting && millis() - lastTransmission > TRANSMIT_INTERVAL) {
        prepareWaveformBuffer();
        startTransmission();
        lastTransmission = millis();
    }
    
    // OLED refresh every 250ms
    if (millis() - lastDisplayUpdate > 250) {
        updateDisplay();
        lastDisplayUpdate = millis();
    }
    
    updateLEDs();
}

void readEnvironment() {
    // Read potentiometers with 30-sample averaging
    int turbidityRaw = readAverage(PIN_TURBIDITY, 30);
    int depthRaw = readAverage(PIN_DEPTH, 30);
    
    envData.turbidityPercent = map(turbidityRaw, 0, 4095, 0, 100);
    envData.depthPercent = map(depthRaw, 0, 4095, 0, 100);
}

void adaptParametersAuto() {
    previousParams = currentParams;
    
    // Turbidity-based automatic waveform selection
    if (envData.turbidityPercent < 20) {
        currentParams.centerFrequency = 500000;
        currentParams.bandwidth = 100000;
        currentParams.pulseDuration = 0.005;
        currentParams.amplitude = 2000;
        currentParams.type = LFM_UP_CHIRP;
        currentParams.typeName = "LFM Up";
        currentParams.environmentMode = "Clear Reef";
    }
    else if (envData.turbidityPercent < 40) {
        currentParams.centerFrequency = 400000;
        currentParams.bandwidth = 80000;
        currentParams.pulseDuration = 0.008;
        currentParams.amplitude = 2500;
        currentParams.type = LFM_DOWN_CHIRP;
        currentParams.typeName = "LFM Dn";
        currentParams.environmentMode = "Moderate";
    }
    else if (envData.turbidityPercent < 60) {
        currentParams.centerFrequency = 300000;
        currentParams.bandwidth = 60000;
        currentParams.pulseDuration = 0.012;
        currentParams.amplitude = 3000;
        currentParams.type = CW_PULSE;
        currentParams.typeName = "CW";
        currentParams.environmentMode = "Muddy Water";
    }
    else if (envData.turbidityPercent < 80) {
        currentParams.centerFrequency = 200000;
        currentParams.bandwidth = 40000;
        currentParams.pulseDuration = 0.015;
        currentParams.amplitude = 3500;
        currentParams.type = PHASE_CODED;
        currentParams.typeName = "Phase";
        currentParams.environmentMode = "Heavy Mud";
    }
    else {
        currentParams.centerFrequency = 100000;
        currentParams.bandwidth = 20000;
        currentParams.pulseDuration = 0.020;
        currentParams.amplitude = 4095;
        currentParams.type = GEOMETRIC_SWEEP;
        currentParams.typeName = "Geo";
        currentParams.environmentMode = "Muddy Estuary";
    }
    
    // Depth-based amplitude and duration adjustment
    if (envData.depthPercent > 70) {
        currentParams.amplitude = min(currentParams.amplitude + 500, 4095);
        currentParams.pulseDuration *= 1.5;
    }
    else if (envData.depthPercent < 30) {
        currentParams.amplitude = max(currentParams.amplitude - 300, 1000);
        currentParams.pulseDuration *= 0.8;
    }
    
    if (currentParams.centerFrequency != previousParams.centerFrequency ||
        currentParams.type != previousParams.type) {
        announceChange();
    }
}

void prepareWaveformBuffer() {
    totalSamples = (int)(SAMPLE_RATE * currentParams.pulseDuration);
    if (totalSamples > DMA_BUFFER_SIZE) totalSamples = DMA_BUFFER_SIZE;
    
    float startFreq = currentParams.centerFrequency - currentParams.bandwidth/2;
    float endFreq = currentParams.centerFrequency + currentParams.bandwidth/2;
    
    // Generate waveform samples
    for(int i = 0; i < totalSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float progress = (float)i / totalSamples;
        float value = 0;
        
        switch(currentParams.type) {
            case CW_PULSE:
                value = sin(2 * PI * currentParams.centerFrequency * t);
                break;
                
            case LFM_UP_CHIRP: {
                float chirpRate = (endFreq - startFreq) / currentParams.pulseDuration;
                float phase = 2 * PI * (startFreq * t + 0.5 * chirpRate * t * t);
                value = sin(phase);
                break;
            }
                
            case LFM_DOWN_CHIRP: {
                float chirpRate = (startFreq - endFreq) / currentParams.pulseDuration;
                float phase = 2 * PI * (endFreq * t + 0.5 * chirpRate * t * t);
                value = sin(phase);
                break;
            }
                
            case PHASE_CODED: {
                int barkerCode[7] = {1, 1, 1, -1, -1, 1, -1};
                int chipIndex = (i * 7) / totalSamples;
                float phaseShift = (barkerCode[chipIndex] == 1) ? 0 : PI;
                value = sin(2 * PI * currentParams.centerFrequency * t + phaseShift);
                break;
            }
                
            case GEOMETRIC_SWEEP: {
                float ratio = pow(endFreq/startFreq, 1.0/totalSamples);
                float freq = startFreq * pow(ratio, i);
                float phase = 2 * PI * freq * t;
                value = sin(phase);
                break;
            }
        }
        
        // Apply digital windowing to reduce sidelobes
        float windowValue = applyWindow(i, totalSamples, currentParams.window);
        value *= windowValue;
        
        currentBuffer[i] = (uint16_t)((value + 1) * (currentParams.amplitude / 2));
    }
    
    bufferReady = true;
}

float applyWindow(int i, int N, WindowType type) {
    float n = (float)i;
    float total = (float)(N - 1);
    
    switch(type) {
        case WINDOW_HAMMING:
            return 0.54 - 0.46 * cos(2 * PI * n / total);
        case WINDOW_HANN:
            return 0.5 - 0.5 * cos(2 * PI * n / total);
        case WINDOW_BLACKMAN:
            return 0.42 - 0.5 * cos(2 * PI * n / total) 
                   + 0.08 * cos(4 * PI * n / total);
        default:
            return 1.0;
    }
}

void startTransmission() {
    if (!bufferReady) return;
    sampleIndex = 0;
    transmitting = true;
    digitalWrite(PIN_LED_TX, HIGH);
}

void updateLEDs() {
    // Frequency-based LED indication
    if (currentParams.centerFrequency >= 400000) {
        digitalWrite(PIN_LED_HIGH_FREQ, HIGH);
        digitalWrite(PIN_LED_LOW_FREQ, LOW);
    }
    else if (currentParams.centerFrequency <= 200000) {
        digitalWrite(PIN_LED_HIGH_FREQ, LOW);
        digitalWrite(PIN_LED_LOW_FREQ, HIGH);
    }
    else {
        digitalWrite(PIN_LED_HIGH_FREQ, LOW);
        digitalWrite(PIN_LED_LOW_FREQ, LOW);
    }
}

void updateDisplay() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    display.println("S.A.L.S.A PAYLOAD");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    display.setCursor(0, 14);
    display.print("ENV: ");
    display.println(currentParams.environmentMode);
    
    display.setCursor(0, 22);
    display.print("FREQ: ");
    display.print(currentParams.centerFrequency/1000, 1);
    display.println(" kHz");
    
    display.setCursor(0, 30);
    display.print("WAVE: ");
    display.print(currentParams.typeName);
    display.print(" + ");
    display.println(currentParams.windowName);
    
    display.setCursor(0, 38);
    display.print("PWR: ");
    int pwr = (currentParams.amplitude / 4095.0) * 100;
    display.print(pwr);
    display.println("%");
    
    display.setCursor(0, 46);
    display.print("DUR: ");
    display.print(currentParams.pulseDuration * 1000, 1);
    display.println(" ms");
    
    display.setCursor(0, 56);
    display.println("AUTO MODE");
    
    display.display();
}

void applyPowerSaving() {
    // Dynamic CPU frequency scaling
    if (currentParams.centerFrequency >= 400000) {
        setCpuFrequencyMhz(240);
    } else if (currentParams.centerFrequency >= 200000) {
        setCpuFrequencyMhz(160);
    } else {
        setCpuFrequencyMhz(80);
    }
}

int readAverage(int pin, int samples) {
    long sum = 0;
    for(int i = 0; i < samples; i++) {
        sum += analogRead(pin);
        delayMicroseconds(100);
    }
    return sum / samples;
}

void announceChange() {
    Serial.println("\nPARAMETER CHANGE DETECTED");
    Serial.print("Environment: ");
    Serial.println(currentParams.environmentMode);
    Serial.print("Frequency: ");
    Serial.print(currentParams.centerFrequency/1000, 1);
    Serial.println(" kHz");
    Serial.print("Waveform: ");
    Serial.println(currentParams.typeName);
    Serial.print("Duration: ");
    Serial.print(currentParams.pulseDuration*1000, 1);
    Serial.println(" ms");
    Serial.print("Amplitude: ");
    Serial.print((currentParams.amplitude/4095.0)*100, 1);
    Serial.println("%");
}