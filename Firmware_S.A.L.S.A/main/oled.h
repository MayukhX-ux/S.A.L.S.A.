#pragma once

#include "esp_err.h"
#include "sonar.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t oled_init();

void oled_clear();

void oled_show_startup();

void oled_update(
    const SonarParameters &parameters
);

#ifdef __cplusplus
}
#endif