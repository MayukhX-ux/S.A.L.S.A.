#include "adc_input.h"
#include "config.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "esp_log.h"

static const char *TAG = "ADC";

static adc_oneshot_unit_handle_t adc_handle = nullptr;

static adc_cali_handle_t cali_handle = nullptr;

static bool calibration_enabled = false;


// ============================================================
// Convert ADC GPIO to ADC channel
// ============================================================

static adc_channel_t gpio_to_channel(int gpio)
{
    switch (gpio)
    {
        case GPIO_TURBIDITY:
            return ADC_CHANNEL_6;

        case GPIO_DEPTH:
            return ADC_CHANNEL_7;

        case GPIO_TEMPERATURE:
            return ADC_CHANNEL_0;

        default:
            return ADC_CHANNEL_0;
    }
}


// ============================================================
// ADC INITIALIZATION
// ============================================================

esp_err_t adc_input_init()
{
    adc_oneshot_unit_init_cfg_t init_config = {};

    init_config.unit_id = ADC_UNIT_1;

    ESP_ERROR_CHECK(
        adc_oneshot_new_unit(
            &init_config,
            &adc_handle
        )
    );

    adc_oneshot_chan_cfg_t channel_config = {};

    channel_config.bitwidth = ADC_BITWIDTH_DEFAULT;
    channel_config.atten = ADC_ATTEN_DB_11;


    // --------------------------------------------------------
    // Turbidity
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(
            adc_handle,
            ADC_CHANNEL_6,
            &channel_config
        )
    );


    // --------------------------------------------------------
    // Depth
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(
            adc_handle,
            ADC_CHANNEL_7,
            &channel_config
        )
    );


    // --------------------------------------------------------
    // Temperature
    // --------------------------------------------------------

    ESP_ERROR_CHECK(
        adc_oneshot_config_channel(
            adc_handle,
            ADC_CHANNEL_0,
            &channel_config
        )
    );


    // --------------------------------------------------------
    // ADC calibration
    // --------------------------------------------------------

    adc_cali_line_fitting_config_t cali_config = {};

    cali_config.unit_id = ADC_UNIT_1;
    cali_config.atten = ADC_ATTEN_DB_11;
    cali_config.bitwidth = ADC_BITWIDTH_DEFAULT;

    esp_err_t cali_result =
        adc_cali_create_scheme_line_fitting(
            &cali_config,
            &cali_handle
        );

    if (cali_result == ESP_OK)
    {
        calibration_enabled = true;
        ESP_LOGI(TAG, "ADC calibration enabled");
    }
    else
    {
        calibration_enabled = false;
        ESP_LOGW(TAG, "ADC calibration unavailable; using raw ADC");
    }

    return ESP_OK;
}


// ============================================================
// Read normalized ADC value
// ============================================================

static float read_normalized(int gpio)
{
    adc_channel_t channel = gpio_to_channel(gpio);

    int raw = 0;

    ESP_ERROR_CHECK(
        adc_oneshot_read(
            adc_handle,
            channel,
            &raw
        )
    );

    int millivolts = 0;

    if (calibration_enabled)
    {
        if (adc_cali_raw_to_voltage(
                cali_handle,
                raw,
                &millivolts) == ESP_OK)
        {
            float normalized =
                static_cast<float>(millivolts) / 3300.0f;

            if (normalized < 0.0f)
                normalized = 0.0f;

            if (normalized > 1.0f)
                normalized = 1.0f;

            return normalized;
        }
    }

    // Fallback
    float normalized =
        static_cast<float>(raw) / 4095.0f;

    if (normalized < 0.0f)
        normalized = 0.0f;

    if (normalized > 1.0f)
        normalized = 1.0f;

    return normalized;
}


// ============================================================
// Turbidity
// ============================================================

float adc_read_turbidity()
{
    return read_normalized(GPIO_TURBIDITY);
}


// ============================================================
// Depth
// ============================================================

float adc_read_depth()
{
    return read_normalized(GPIO_DEPTH);
}


// ============================================================
// Temperature
// ============================================================
//
// Pot position:
// 0.0 -> TEMP_MIN_C
// 1.0 -> TEMP_MAX_C
//

float adc_read_temperature_c()
{
    float normalized =
        read_normalized(GPIO_TEMPERATURE);

    float temperature =
        TEMP_MIN_C +
        normalized *
        (TEMP_MAX_C - TEMP_MIN_C);

    return temperature;
}


// ============================================================
// Read all environmental parameters
// ============================================================

EnvironmentalData adc_read_all()
{
    EnvironmentalData data;

    data.turbidity =
        adc_read_turbidity();

    data.depth =
        adc_read_depth();

    data.temperature_c =
        adc_read_temperature_c();

    return data;
}