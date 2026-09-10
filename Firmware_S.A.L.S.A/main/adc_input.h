#pragma once

#include <stdint.h>
#include "esp_err.h"

struct EnvironmentalData
{
    float turbidity;
    float depth;
    float temperature_c;
};

esp_err_t adc_input_init();

float adc_read_turbidity();

float adc_read_depth();

float adc_read_temperature_c();

EnvironmentalData adc_read_all();