#pragma once

#include <stdint.h>

// ============================================================
// ENVIRONMENTAL SENSOR / POTENTIOMETER INPUTS
// ============================================================

#define GPIO_TURBIDITY     34      // ADC1_CH6
#define GPIO_DEPTH         35      // ADC1_CH7
#define GPIO_TEMPERATURE   36      // ADC1_CH0

// ============================================================
// OLED
// ============================================================

#define GPIO_OLED_SDA      32
#define GPIO_OLED_SCL      33

#define OLED_I2C_ADDR      0x3C

// ============================================================
// 12-BIT R-2R DAC
// ============================================================
//
// D0 = LSB
// D11 = MSB
//
// IMPORTANT:
// These GPIOs must remain dedicated to the R-2R ladder.
//

static constexpr int R2R_PINS[12] = {
    4,      // D0
    5,      // D1
    13,     // D2
    14,     // D3
    18,     // D4
    19,     // D5
    21,     // D6
    22,     // D7
    23,     // D8
    25,     // D9
    26,     // D10
    27      // D11
};

// ============================================================
// SAMPLE RATE
// ============================================================

#define SAMPLE_RATE_HZ     4000000UL

// ============================================================
// SONAR FREQUENCY LIMITS
// ============================================================

#define FREQ_MIN_HZ        100000.0f
#define FREQ_MAX_HZ        500000.0f

#define BW_MIN_HZ          100000.0f
#define BW_MAX_HZ          400000.0f

// ============================================================
// PULSE PARAMETERS
// ============================================================

#define PULSE_MIN_US       200.0f
#define PULSE_MAX_US       1000.0f

// ============================================================
// OUTPUT AMPLITUDE
// ============================================================

#define AMP_MIN            0.10f
#define AMP_MAX            0.80f

// ============================================================
// WAVEFORM BUFFER
// ============================================================

#define WAVE_BUFFER_SAMPLES    8192

// ============================================================
// ENVIRONMENTAL WEIGHTS
// ============================================================
//
// Prototype adaptation policy:
//
// Turbidity    -> 45%
// Depth        -> 30%
// Temperature  -> 25%
//
// Total = 100%
//

#define TURBIDITY_WEIGHT       0.45f
#define DEPTH_WEIGHT           0.30f
#define TEMPERATURE_WEIGHT     0.25f

// ============================================================
// TEMPERATURE RANGE USED BY THE DEMO POTENTIOMETER
// ============================================================
//
// The potentiometer represents approximately:
//
// 0%   -> 5 °C
// 100% -> 35 °C
//
// Change these later if your intended operating environment
// requires another range.
//

#define TEMP_MIN_C             5.0f
#define TEMP_MAX_C             35.0f

// ============================================================
// ENVIRONMENTAL REFERENCE TEMPERATURE
// ============================================================
//
// Around this value, temperature has the smallest penalty.
// Moving toward either extreme increases the temperature
// stress factor.
//

#define TEMP_REFERENCE_C       20.0f

// ============================================================
// WAVEFORM MODES
// ============================================================

enum class WaveformMode : uint8_t
{
    LFM = 0,
    GEOMETRIC = 1,
    BARKER7 = 2
};