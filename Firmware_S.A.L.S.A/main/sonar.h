#pragma once

#include <stdint.h>
#include <stddef.h>

#include "config.h"

struct SonarParameters
{
    // Raw environmental inputs
    float turbidity;
    float depth;
    float temperature_c;

    // Combined environmental index
    float environment;

    // Adapted sonar parameters
    float f0_hz;
    float f1_hz;
    float center_hz;
    float bandwidth_hz;
    float pulse_us;
    float amplitude;

    WaveformMode mode;
};


// ============================================================
// Initialization
// ============================================================

void sonar_init();


// ============================================================
// Environmental adaptation
// ============================================================

SonarParameters sonar_adapt(
    float turbidity,
    float depth,
    float temperature_c
);


// ============================================================
// Waveform generation
// ============================================================

size_t sonar_generate(
    uint16_t *buffer,
    size_t capacity,
    const SonarParameters &parameters
);


// ============================================================
// Windowing
// ============================================================

void sonar_apply_hann(
    uint16_t *buffer,
    size_t samples
);

void sonar_apply_hamming(
    uint16_t *buffer,
    size_t samples
);

void sonar_apply_blackman(
    uint16_t *buffer,
    size_t samples
);


// ============================================================
// Utility
// ============================================================

const char *sonar_mode_name(
    WaveformMode mode
);