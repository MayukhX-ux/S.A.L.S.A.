#include "sonar.h"

#include <cmath>
#include <algorithm>

static constexpr int LUT_SIZE = 4096;

static float sine_lut[LUT_SIZE];


// ============================================================
// Utility
// ============================================================

static float clamp01(float value)
{
    if (value < 0.0f)
        return 0.0f;

    if (value > 1.0f)
        return 1.0f;

    return value;
}


// ============================================================
// Temperature stress factor
// ============================================================
//
// At approximately 20 °C:
//     stress = 0
//
// Toward 5 °C or 35 °C:
//     stress approaches 1
//
// This is a prototype adaptation policy, not a universal
// ocean-acoustic propagation model.
//

static float temperature_stress(float temperature_c)
{
    float distance =
        std::fabs(
            temperature_c - TEMP_REFERENCE_C
        );

    float maximum_distance =
        std::max(
            TEMP_REFERENCE_C - TEMP_MIN_C,
            TEMP_MAX_C - TEMP_REFERENCE_C
        );

    if (maximum_distance <= 0.0f)
        return 0.0f;

    return clamp01(
        distance / maximum_distance
    );
}


// ============================================================
// DDS phase increment
// ============================================================

static uint32_t phase_increment(float frequency_hz)
{
    const double scale =
        4294967296.0 /
        static_cast<double>(SAMPLE_RATE_HZ);

    return static_cast<uint32_t>(
        frequency_hz * scale
    );
}


// ============================================================
// LUT sine
// ============================================================

static float sine_from_lut(uint32_t phase)
{
    uint32_t index =
        phase >>
        (32 - 12);

    return sine_lut[index & (LUT_SIZE - 1)];
}


// ============================================================
// Convert bipolar waveform to 12-bit DAC
// ============================================================

static uint16_t bipolar_to_dac(
    float value,
    float amplitude
)
{
    value *= amplitude;

    if (value > 1.0f)
        value = 1.0f;

    if (value < -1.0f)
        value = -1.0f;

    float normalized =
        (value + 1.0f) * 0.5f;

    uint32_t dac =
        static_cast<uint32_t>(
            normalized * 4095.0f
        );

    if (dac > 4095)
        dac = 4095;

    return static_cast<uint16_t>(dac);
}


// ============================================================
// Initialization
// ============================================================

void sonar_init()
{
    for (int i = 0; i < LUT_SIZE; i++)
    {
        float phase =
            (2.0f * M_PI * i) /
            LUT_SIZE;

        sine_lut[i] =
            std::sin(phase);
    }
}


// ============================================================
// Environmental adaptation
// ============================================================

SonarParameters sonar_adapt(
    float turbidity,
    float depth,
    float temperature_c
)
{
    SonarParameters p = {};

    turbidity =
        clamp01(turbidity);

    depth =
        clamp01(depth);

    temperature_c =
        std::clamp(
            temperature_c,
            TEMP_MIN_C,
            TEMP_MAX_C
        );


    // --------------------------------------------------------
    // Temperature environmental stress
    // --------------------------------------------------------

    float temperature_factor =
        temperature_stress(
            temperature_c
        );


    // --------------------------------------------------------
    // Combined environmental index
    // --------------------------------------------------------
    //
    // Turbidity  = 45%
    // Depth      = 30%
    // Temperature = 25%
    //

    float environment =
        TURBIDITY_WEIGHT * turbidity +
        DEPTH_WEIGHT * depth +
        TEMPERATURE_WEIGHT *
            temperature_factor;

    environment =
        clamp01(environment);

    p.turbidity =
        turbidity;

    p.depth =
        depth;

    p.temperature_c =
        temperature_c;

    p.environment =
        environment;


    // ========================================================
    // Adaptive bandwidth
    // ========================================================
    //
    // Clean / favorable:
    //     approximately 400 kHz
    //
    // Difficult environment:
    //     approximately 100 kHz
    //

    p.bandwidth_hz =
        BW_MAX_HZ -
        300000.0f *
        environment;


    // ========================================================
    // Adaptive center frequency
    // ========================================================

    p.center_hz =
        300000.0f -
        100000.0f *
        environment;


    // ========================================================
    // Calculate chirp endpoints
    // ========================================================

    p.f0_hz =
        p.center_hz -
        p.bandwidth_hz * 0.5f;

    p.f1_hz =
        p.center_hz +
        p.bandwidth_hz * 0.5f;


    // Safety limits

    p.f0_hz =
        std::clamp(
            p.f0_hz,
            FREQ_MIN_HZ,
            FREQ_MAX_HZ
        );

    p.f1_hz =
        std::clamp(
            p.f1_hz,
            FREQ_MIN_HZ,
            FREQ_MAX_HZ
        );


    // ========================================================
    // Adaptive pulse duration
    // ========================================================

    p.pulse_us =
        PULSE_MIN_US +
        (
            PULSE_MAX_US -
            PULSE_MIN_US
        ) *
        environment;


    // ========================================================
    // Adaptive amplitude
    // ========================================================

    p.amplitude =
        0.80f -
        0.45f *
        environment;

    p.amplitude =
        std::clamp(
            p.amplitude,
            AMP_MIN,
            AMP_MAX
        );


    // Default waveform

    p.mode =
        WaveformMode::LFM;

    return p;
}


// ============================================================
// Waveform generation
// ============================================================

size_t sonar_generate(
    uint16_t *buffer,
    size_t capacity,
    const SonarParameters &p
)
{
    if (buffer == nullptr)
        return 0;

    size_t samples =
        static_cast<size_t>(
            (
                p.pulse_us *
                static_cast<float>(
                    SAMPLE_RATE_HZ
                )
            ) /
            1000000.0f
        );

    if (samples > capacity)
        samples = capacity;

    if (samples == 0)
        return 0;


    // ========================================================
    // LFM CHIRP
    // ========================================================

    if (p.mode == WaveformMode::LFM)
    {
        uint32_t phase = 0;

        double phase_increment_start =
            (
                static_cast<double>(p.f0_hz) *
                4294967296.0
            ) /
            SAMPLE_RATE_HZ;

        double frequency_slope =
            (
                static_cast<double>(
                    p.f1_hz - p.f0_hz
                )
            ) /
            static_cast<double>(
                samples
            );

        double phase_increment_current =
            phase_increment_start;

        for (size_t n = 0; n < samples; n++)
        {
            float value =
                sine_from_lut(phase);

            buffer[n] =
                bipolar_to_dac(
                    value,
                    p.amplitude
                );

            phase +=
                static_cast<uint32_t>(
                    phase_increment_current
                );

            phase_increment_current +=
                (
                    frequency_slope *
                    4294967296.0
                ) /
                SAMPLE_RATE_HZ;
        }
    }


    // ========================================================
    // GEOMETRIC SWEEP
    // ========================================================

    else if (p.mode == WaveformMode::GEOMETRIC)
    {
        uint32_t phase = 0;

        float ratio =
            p.f1_hz /
            p.f0_hz;

        for (size_t n = 0; n < samples; n++)
        {
            float u =
                static_cast<float>(n) /
                static_cast<float>(
                    samples - 1
                );

            float frequency =
                p.f0_hz *
                std::pow(
                    ratio,
                    u
                );

            phase +=
                phase_increment(
                    frequency
                );

            float value =
                sine_from_lut(phase);

            buffer[n] =
                bipolar_to_dac(
                    value,
                    p.amplitude
                );
        }
    }


    // ========================================================
    // BARKER-7
    // ========================================================

    else if (p.mode == WaveformMode::BARKER7)
    {
        static const int8_t barker[7] =
        {
            +1,
            +1,
            +1,
            -1,
            -1,
            +1,
            -1
        };

        uint32_t phase = 0;

        uint32_t inc =
            phase_increment(
                p.center_hz
            );

        size_t chip_length =
            samples / 7;

        if (chip_length == 0)
            chip_length = 1;

        for (size_t n = 0; n < samples; n++)
        {
            size_t chip =
                n / chip_length;

            if (chip > 6)
                chip = 6;

            float value =
                sine_from_lut(phase);

            value *=
                static_cast<float>(
                    barker[chip]
                );

            buffer[n] =
                bipolar_to_dac(
                    value,
                    p.amplitude
                );

            phase += inc;
        }
    }

    return samples;
}


// ============================================================
// Hann window
// ============================================================

void sonar_apply_hann(
    uint16_t *buffer,
    size_t samples
)
{
    if (samples < 2)
        return;

    for (size_t n = 0; n < samples; n++)
    {
        float w =
            0.5f *
            (
                1.0f -
                std::cos(
                    2.0f *
                    M_PI *
                    static_cast<float>(n) /
                    static_cast<float>(samples - 1)
                )
            );

        float normalized =
            static_cast<float>(
                buffer[n]
            ) /
            4095.0f;

        normalized *= w;

        buffer[n] =
            static_cast<uint16_t>(
                normalized * 4095.0f
            );
    }
}


// ============================================================
// Hamming window
// ============================================================

void sonar_apply_hamming(
    uint16_t *buffer,
    size_t samples
)
{
    if (samples < 2)
        return;

    for (size_t n = 0; n < samples; n++)
    {
        float w =
            0.54f -
            0.46f *
            std::cos(
                2.0f *
                M_PI *
                static_cast<float>(n) /
                static_cast<float>(samples - 1)
            );

        float normalized =
            static_cast<float>(
                buffer[n]
            ) /
            4095.0f;

        normalized *= w;

        buffer[n] =
            static_cast<uint16_t>(
                normalized * 4095.0f
            );
    }
}


// ============================================================
// Blackman window
// ============================================================

void sonar_apply_blackman(
    uint16_t *buffer,
    size_t samples
)
{
    if (samples < 2)
        return;

    constexpr float alpha = 0.16f;

    for (size_t n = 0; n < samples; n++)
    {
        float x =
            static_cast<float>(n) /
            static_cast<float>(samples - 1);

        float w =
            (
                (1.0f - alpha) * 0.5f
            )
            -
            0.5f *
            std::cos(
                2.0f * M_PI * x
            )
            +
            (alpha * 0.5f) *
            std::cos(
                4.0f * M_PI * x
            );

        float normalized =
            static_cast<float>(
                buffer[n]
            ) /
            4095.0f;

        normalized *= w;

        buffer[n] =
            static_cast<uint16_t>(
                normalized * 4095.0f
            );
    }
}


// ============================================================
// Mode name
// ============================================================

const char *sonar_mode_name(
    WaveformMode mode
)
{
    switch (mode)
    {
        case WaveformMode::LFM:
            return "LFM";

        case WaveformMode::GEOMETRIC:
            return "GEOMETRIC";

        case WaveformMode::BARKER7:
            return "BARKER-7";

        default:
            return "UNKNOWN";
    }
}