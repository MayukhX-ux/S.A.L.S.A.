idf_component_register(
    SRCS
        "main.cpp"
        "sonar.cpp"
        "adc_input.cpp"
        "oled.cpp"

    INCLUDE_DIRS
        "."

    REQUIRES
        driver
        esp_adc
        esp_timer
        esp_hw_support
        parallel_i2s
)