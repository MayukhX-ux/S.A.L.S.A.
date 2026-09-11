#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_err.h"

#include "config.h"
#include "adc_input.h"
#include "sonar.h"
#include "parallel_i2s.h"
#include "oled.h"



// TAG


static const char *TAG = "SALSA";



// APPLICATION CONFIGURATION


// How frequently the OLED is updated
static constexpr uint32_t OLED_UPDATE_PERIOD_MS = 250;

// Temporary demonstration transmit time
//
// This is NOT the final deterministic PRI implementation.
// Hardware timer based transmit gating should be added during
// the final hardware stage.
static constexpr uint32_t DEMO_TX_TIME_MS = 100;

// Delay between transmissions
static constexpr uint32_t DEMO_TX_GAP_MS = 10;



// MAIN APPLICATION


extern "C" void app_main()
{
    
    // STARTUP INFORMATION
    

    ESP_LOGI(
        TAG,
        "=============================================="
    );

    ESP_LOGI(
        TAG,
        " S.A.L.S.A."
    );

    ESP_LOGI(
        TAG,
        " Software Defined Adaptive Sonar Transmitter"
    );

    ESP_LOGI(
        TAG,
        "=============================================="
    );

    ESP_LOGI(
        TAG,
        "Target MCU : ESP32-WROOM-32"
    );

    ESP_LOGI(
        TAG,
        "Sample Rate: %lu Hz",
        SAMPLE_RATE_HZ
    );

    ESP_LOGI(
        TAG,
        "Frequency  : %.0f - %.0f kHz",
        FREQ_MIN_HZ / 1000.0f,
        FREQ_MAX_HZ / 1000.0f
    );

    ESP_LOGI(
        TAG,
        "DAC        : 12-bit R-2R"
    );

    ESP_LOGI(
        TAG,
        "Output     : I2S0 Parallel + DMA"
    );


    
    // INITIALIZE SONAR ENGINE
    

    ESP_LOGI(
        TAG,
        "Initializing sonar engine..."
    );

    sonar_init();

    ESP_LOGI(
        TAG,
        "Sonar engine initialized"
    );


    
    // INITIALIZE ADC
    

    ESP_LOGI(
        TAG,
        "Initializing environmental ADC..."
    );

    esp_err_t err =
        adc_input_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "ADC initialization failed: %s",
            esp_err_to_name(err)
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "Environmental ADC initialized"
    );

    ESP_LOGI(
        TAG,
        "Turbidity   -> GPIO%d",
        GPIO_TURBIDITY
    );

    ESP_LOGI(
        TAG,
        "Depth       -> GPIO%d",
        GPIO_DEPTH
    );

    ESP_LOGI(
        TAG,
        "Temperature -> GPIO%d",
        GPIO_TEMPERATURE
    );


    
    // INITIALIZE I2S PARALLEL OUTPUT
    

    ESP_LOGI(
        TAG,
        "Initializing I2S0 parallel output..."
    );

    err =
        parallel_i2s_init(
            SAMPLE_RATE_HZ
        );

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "I2S initialization failed: %s",
            esp_err_to_name(err)
        );

        return;
    }

    ESP_LOGI(
        TAG,
        "I2S0 parallel output initialized"
    );


    
    // INITIALIZE OLED
    

    ESP_LOGI(
        TAG,
        "Initializing SSD1306 OLED..."
    );

    err =
        oled_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "OLED initialization failed: %s",
            esp_err_to_name(err)
        );

        
        // OLED is considered a user-interface peripheral.
        //
        // The sonar system can still operate without it.
        // Therefore we DON'T terminate the application here.
        

        ESP_LOGW(
            TAG,
            "Continuing without OLED"
        );
    }
    else
    {
        ESP_LOGI(
            TAG,
            "OLED initialized successfully"
        );
    }


    
    // ALLOCATE DMA-CAPABLE WAVEFORM BUFFER
    

    ESP_LOGI(
        TAG,
        "Allocating waveform buffer..."
    );

    uint16_t *waveform_buffer =
        static_cast<uint16_t *>(
            heap_caps_malloc(
                WAVE_BUFFER_SAMPLES *
                sizeof(uint16_t),

                MALLOC_CAP_DMA |
                MALLOC_CAP_8BIT
            )
        );


    
    // CHECK BUFFER
    

    if (waveform_buffer == nullptr)
    {
        ESP_LOGE(
            TAG,
            "Failed to allocate DMA waveform buffer"
        );

        parallel_i2s_stop();

        return;
    }


    ESP_LOGI(
        TAG,
        "Waveform buffer allocated"
    );

    ESP_LOGI(
        TAG,
        "Buffer samples: %d",
        WAVE_BUFFER_SAMPLES
    );

    ESP_LOGI(
        TAG,
        "Buffer size: %u bytes",
        static_cast<unsigned>(
            WAVE_BUFFER_SAMPLES *
            sizeof(uint16_t)
        )
    );


    
    // CLEAR WAVEFORM BUFFER
    

    for (size_t i = 0;
         i < WAVE_BUFFER_SAMPLES;
         i++)
    {
        waveform_buffer[i] = 2048;
    }


    
    // OLED UPDATE TIMER
    

    TickType_t last_oled_update =
        xTaskGetTickCount();


    
    // MAIN CONTROL LOOP
    

    while (true)
    {
        
        // 1. READ ENVIRONMENTAL PARAMETERS
        

        EnvironmentalData environment =
            adc_read_all();


        
        // 2. ADAPT SONAR PARAMETERS
        

        SonarParameters parameters =
            sonar_adapt(
                environment.turbidity,
                environment.depth,
                environment.temperature_c
            );


        
        // 3. UPDATE OLED
        

        TickType_t now =
            xTaskGetTickCount();


        if (
            (
                now -
                last_oled_update
            )
            >=
            pdMS_TO_TICKS(
                OLED_UPDATE_PERIOD_MS
            )
        )
        {
            
            // OLED update is deliberately slow.
            //
            // It does NOT participate in the high-speed
            // waveform/DMA path.
            

            oled_update(
                parameters
            );

            last_oled_update =
                now;
        }


        
        // 4. LOG ENVIRONMENTAL DATA
        

        ESP_LOGI(
            TAG,
            "----------------------------------------------"
        );

        ESP_LOGI(
            TAG,
            "ENVIRONMENT"
        );

        ESP_LOGI(
            TAG,
            "Turbidity   : %.3f",
            parameters.turbidity
        );

        ESP_LOGI(
            TAG,
            "Depth       : %.3f",
            parameters.depth
        );

        ESP_LOGI(
            TAG,
            "Temperature : %.2f C",
            parameters.temperature_c
        );

        ESP_LOGI(
            TAG,
            "Environment : %.3f",
            parameters.environment
        );


        
        // 5. LOG ADAPTED SONAR PARAMETERS
        

        ESP_LOGI(
            TAG,
            "SONAR PARAMETERS"
        );

        ESP_LOGI(
            TAG,
            "Mode        : %s",
            sonar_mode_name(
                parameters.mode
            )
        );

        ESP_LOGI(
            TAG,
            "F0          : %.2f kHz",
            parameters.f0_hz / 1000.0f
        );

        ESP_LOGI(
            TAG,
            "F1          : %.2f kHz",
            parameters.f1_hz / 1000.0f
        );

        ESP_LOGI(
            TAG,
            "Center      : %.2f kHz",
            parameters.center_hz / 1000.0f
        );

        ESP_LOGI(
            TAG,
            "Bandwidth   : %.2f kHz",
            parameters.bandwidth_hz / 1000.0f
        );

        ESP_LOGI(
            TAG,
            "Pulse       : %.2f us",
            parameters.pulse_us
        );

        ESP_LOGI(
            TAG,
            "Amplitude   : %.3f",
            parameters.amplitude
        );


        
        // 6. GENERATE WAVEFORM
        

        size_t generated_samples =
            sonar_generate(
                waveform_buffer,
                WAVE_BUFFER_SAMPLES,
                parameters
            );


        
        // 7. CHECK WAVEFORM GENERATION
        

        if (generated_samples == 0)
        {
            ESP_LOGE(
                TAG,
                "Waveform generation failed"
            );

            vTaskDelay(
                pdMS_TO_TICKS(100)
            );

            continue;
        }


        
        // 8. APPLY HANN WINDOW
        

        sonar_apply_hann(
            waveform_buffer,
            generated_samples
        );


        
        // 9. LOG WAVEFORM SIZE
        

        ESP_LOGI(
            TAG,
            "Generated samples: %u",
            static_cast<unsigned>(
                generated_samples
            )
        );

        ESP_LOGI(
            TAG,
            "Waveform duration: %.2f us",
            (
                static_cast<float>(
                    generated_samples
                )
                /
                static_cast<float>(
                    SAMPLE_RATE_HZ
                )
            )
            * 1000000.0f
        );


        
        // 10. START I2S DMA TRANSMISSION
        

        ESP_LOGI(
            TAG,
            "Starting I2S DMA transmission..."
        );


        err =
            parallel_i2s_start(
                waveform_buffer,
                generated_samples
            );


        if (err != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "I2S DMA start failed: %s",
                esp_err_to_name(err)
            );

            vTaskDelay(
                pdMS_TO_TICKS(100)
            );

            continue;
        }


        
        // 11. DEMONSTRATION TRANSMISSION PERIOD
        
        //
        // IMPORTANT:
        //
        // This delay is only for the current firmware
        // demonstration.
        //
        // In the final hardware implementation we should
        // replace this with deterministic hardware-timer
        // controlled transmit gating / PRI.
        //

        vTaskDelay(
            pdMS_TO_TICKS(
                DEMO_TX_TIME_MS
            )
        );


        
        // 12. STOP I2S DMA
        

        err =
            parallel_i2s_stop();


        if (err != ESP_OK)
        {
            ESP_LOGE(
                TAG,
                "I2S DMA stop failed: %s",
                esp_err_to_name(err)
            );
        }


        
        // 13. WAIT BEFORE NEXT ENVIRONMENTAL UPDATE
        

        vTaskDelay(
            pdMS_TO_TICKS(
                DEMO_TX_GAP_MS
            )
        );
    }


    
    // CLEANUP
    
    //
    // Normally unreachable because app_main() runs forever.
    //

    free(
        waveform_buffer
    );
}
