idf_component_register(
    SRCS
        "parallel_i2s.cpp"

    INCLUDE_DIRS
        "include"

    REQUIRES
        driver
        esp_hw_support
)