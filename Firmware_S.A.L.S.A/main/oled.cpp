#include "oled.h"
#include "config.h"

#include "driver/i2c.h"
#include "esp_log.h"

#include <stdio.h>
#include <string.h>

static const char *TAG = "OLED";

// ============================================================
// SSD1306 configuration
// ============================================================

#define OLED_WIDTH       128
#define OLED_HEIGHT       64
#define OLED_PAGES         8

#define I2C_PORT       I2C_NUM_0

#define I2C_FREQ_HZ    400000

#define SSD1306_ADDR   0x3C

#define OLED_CMD       0x00
#define OLED_DATA      0x40


// ============================================================
// Framebuffer
// ============================================================

static uint8_t framebuffer[
    OLED_WIDTH * OLED_HEIGHT / 8
];


// ============================================================
// 5x7 font
// ============================================================
//
// Characters supported:
// Numbers, uppercase letters, space and basic symbols.
//

static const uint8_t font5x7[][5] =
{
    // Space 32
    {0x00,0x00,0x00,0x00,0x00},

    // !
    {0x00,0x00,0x5F,0x00,0x00},

    // "
    {0x00,0x07,0x00,0x07,0x00},

    // #
    {0x14,0x7F,0x14,0x7F,0x14},

    // $
    {0x24,0x2A,0x7F,0x2A,0x12},

    // %
    {0x23,0x13,0x08,0x64,0x62},

    // &
    {0x36,0x49,0x55,0x22,0x50},

    // '
    {0x00,0x05,0x03,0x00,0x00},

    // (
    {0x00,0x1C,0x22,0x41,0x00},

    // )
    {0x00,0x41,0x22,0x1C,0x00},

    // *
    {0x14,0x08,0x3E,0x08,0x14},

    // +
    {0x08,0x08,0x3E,0x08,0x08},

    // ,
    {0x00,0x50,0x30,0x00,0x00},

    // -
    {0x08,0x08,0x08,0x08,0x08},

    // .
    {0x00,0x60,0x60,0x00,0x00},

    // /
    {0x20,0x10,0x08,0x04,0x02},

    // 0
    {0x3E,0x51,0x49,0x45,0x3E},

    // 1
    {0x00,0x42,0x7F,0x40,0x00},

    // 2
    {0x42,0x61,0x51,0x49,0x46},

    // 3
    {0x21,0x41,0x45,0x4B,0x31},

    // 4
    {0x18,0x14,0x12,0x7F,0x10},

    // 5
    {0x27,0x45,0x45,0x45,0x39},

    // 6
    {0x3C,0x4A,0x49,0x49,0x30},

    // 7
    {0x01,0x71,0x09,0x05,0x03},

    // 8
    {0x36,0x49,0x49,0x49,0x36},

    // 9
    {0x06,0x49,0x49,0x29,0x1E},

    // :
    {0x00,0x36,0x36,0x00,0x00},

    // ;
    {0x00,0x56,0x36,0x00,0x00},

    // <
    {0x08,0x14,0x22,0x41,0x00},

    // =
    {0x14,0x14,0x14,0x14,0x14},

    // >
    {0x00,0x41,0x22,0x14,0x08},

    // ?
    {0x02,0x01,0x51,0x09,0x06},

    // @
    {0x32,0x49,0x79,0x41,0x3E},

    // A
    {0x7E,0x11,0x11,0x11,0x7E},

    // B
    {0x7F,0x49,0x49,0x49,0x36},

    // C
    {0x3E,0x41,0x41,0x41,0x22},

    // D
    {0x7F,0x41,0x41,0x22,0x1C},

    // E
    {0x7F,0x49,0x49,0x49,0x41},

    // F
    {0x7F,0x09,0x09,0x09,0x01},

    // G
    {0x3E,0x41,0x49,0x49,0x7A},

    // H
    {0x7F,0x08,0x08,0x08,0x7F},

    // I
    {0x00,0x41,0x7F,0x41,0x00},

    // J
    {0x20,0x40,0x41,0x3F,0x01},

    // K
    {0x7F,0x08,0x14,0x22,0x41},

    // L
    {0x7F,0x40,0x40,0x40,0x40},

    // M
    {0x7F,0x02,0x0C,0x02,0x7F},

    // N
    {0x7F,0x04,0x08,0x10,0x7F},

    // O
    {0x3E,0x41,0x41,0x41,0x3E},

    // P
    {0x7F,0x09,0x09,0x09,0x06},

    // Q
    {0x3E,0x41,0x51,0x21,0x5E},

    // R
    {0x7F,0x09,0x19,0x29,0x46},

    // S
    {0x46,0x49,0x49,0x49,0x31},

    // T
    {0x01,0x01,0x7F,0x01,0x01},

    // U
    {0x3F,0x40,0x40,0x40,0x3F},

    // V
    {0x1F,0x20,0x40,0x20,0x1F},

    // W
    {0x3F,0x40,0x38,0x40,0x3F},

    // X
    {0x63,0x14,0x08,0x14,0x63},

    // Y
    {0x07,0x08,0x70,0x08,0x07},

    // Z
    {0x61,0x51,0x49,0x45,0x43}
};


// ============================================================
// I2C write
// ============================================================

static esp_err_t i2c_write(
    uint8_t control,
    const uint8_t *data,
    size_t length
)
{
    uint8_t buffer[17];

    size_t offset = 0;

    while (offset < length)
    {
        size_t chunk =
            (length - offset > 16)
                ? 16
                : length - offset;

        buffer[0] = control;

        memcpy(
            &buffer[1],
            &data[offset],
            chunk
        );

        esp_err_t err =
            i2c_master_write_to_device(
                I2C_PORT,
                SSD1306_ADDR,
                buffer,
                chunk + 1,
                pdMS_TO_TICKS(100)
            );

        if (err != ESP_OK)
            return err;

        offset += chunk;
    }

    return ESP_OK;
}


// ============================================================
// Send command
// ============================================================

static esp_err_t oled_command(
    uint8_t command
)
{
    return i2c_write(
        OLED_CMD,
        &command,
        1
    );
}


// ============================================================
// Initialize I2C
// ============================================================

static esp_err_t oled_i2c_init()
{
    i2c_config_t config = {};

    config.mode =
        I2C_MODE_MASTER;

    config.sda_io_num =
        static_cast<gpio_num_t>(
            GPIO_OLED_SDA
        );

    config.scl_io_num =
        static_cast<gpio_num_t>(
            GPIO_OLED_SCL
        );

    config.sda_pullup_en =
        GPIO_PULLUP_ENABLE;

    config.scl_pullup_en =
        GPIO_PULLUP_ENABLE;

    config.master.clk_speed =
        I2C_FREQ_HZ;

    config.clk_flags = 0;

    esp_err_t err =
        i2c_param_config(
            I2C_PORT,
            &config
        );

    if (err != ESP_OK)
        return err;

    return i2c_driver_install(
        I2C_PORT,
        I2C_MODE_MASTER,
        0,
        0,
        0
    );
}


// ============================================================
// SSD1306 initialization
// ============================================================

static esp_err_t oled_controller_init()
{
    const uint8_t commands[] =
    {
        0xAE,       // Display OFF

        0xD5, 0x80, // Clock divide
        0xA8, 0x3F, // Multiplex 1/64
        0xD3, 0x00, // Display offset
        0x40,       // Start line 0

        0x8D, 0x14, // Charge pump

        0x20, 0x00, // Horizontal addressing mode

        0xA1,       // Segment remap
        0xC8,       // COM scan direction

        0xDA, 0x12, // COM pins

        0x81, 0x7F, // Contrast

        0xD9, 0xF1, // Pre-charge
        0xDB, 0x40, // VCOM detect

        0xA4,       // Resume RAM display
        0xA6,       // Normal display

        0xAF        // Display ON
    };

    for (size_t i = 0;
         i < sizeof(commands);
         i++)
    {
        esp_err_t err =
            oled_command(commands[i]);

        if (err != ESP_OK)
            return err;
    }

    return ESP_OK;
}


// ============================================================
// Set cursor
// ============================================================

static void oled_set_cursor(
    uint8_t x,
    uint8_t page
)
{
    oled_command(
        0xB0 | page
    );

    oled_command(
        0x00 | (x & 0x0F)
    );

    oled_command(
        0x10 | ((x >> 4) & 0x0F)
    );
}


// ============================================================
// Clear framebuffer
// ============================================================

void oled_clear()
{
    memset(
        framebuffer,
        0,
        sizeof(framebuffer)
    );
}


// ============================================================
// Draw character
// ============================================================

static void draw_char(
    uint8_t x,
    uint8_t page,
    char character
)
{
    if (character < 32 ||
        character > 90)
    {
        character = ' ';
    }

    uint8_t index =
        character - 32;

    uint8_t data[6];

    for (int i = 0; i < 5; i++)
    {
        data[i] =
            font5x7[index][i];
    }

    data[5] = 0x00;

    oled_set_cursor(
        x,
        page
    );

    i2c_write(
        OLED_DATA,
        data,
        6
    );
}


// ============================================================
// Draw string
// ============================================================

static void draw_string(
    uint8_t x,
    uint8_t page,
    const char *text
)
{
    while (*text != '\0')
    {
        if (x > 122)
            break;

        draw_char(
            x,
            page,
            *text
        );

        x += 6;

        text++;
    }
}


// ============================================================
// Refresh framebuffer
// ============================================================
//
// Currently the text drawing directly writes to the display.
// The framebuffer is retained for future graphics expansion.
//

static void oled_refresh()
{
    for (uint8_t page = 0;
         page < OLED_PAGES;
         page++)
    {
        oled_set_cursor(
            0,
            page
        );

        i2c_write(
            OLED_DATA,
            &framebuffer[
                page * OLED_WIDTH
            ],
            OLED_WIDTH
        );
    }
}


// ============================================================
// Startup screen
// ============================================================

void oled_show_startup()
{
    oled_clear();

    draw_string(
        22,
        0,
        "S.A.L.S.A."
    );

    draw_string(
        16,
        2,
        "ADAPTIVE"
    );

    draw_string(
        10,
        4,
        "SONAR TX"
    );

    draw_string(
        28,
        6,
        "READY"
    );
}


// ============================================================
// Main OLED update
// ============================================================

void oled_update(
    const SonarParameters &p
)
{
    char line[24];

    // --------------------------------------------------------
    // Clear display
    // --------------------------------------------------------

    oled_command(0xAE);

    oled_clear();


    // --------------------------------------------------------
    // Line 1
    // --------------------------------------------------------

    draw_string(
        0,
        0,
        "S.A.L.S.A. STATUS"
    );


    // --------------------------------------------------------
    // Line 2 - Temperature
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "TEMP: %.1f C",
        p.temperature_c
    );

    draw_string(
        0,
        1,
        line
    );


    // --------------------------------------------------------
    // Line 3 - Turbidity
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "TURB: %.2f",
        p.turbidity
    );

    draw_string(
        0,
        2,
        line
    );


    // --------------------------------------------------------
    // Line 4 - Depth
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "DEPTH: %.2f",
        p.depth
    );

    draw_string(
        0,
        3,
        line
    );


    // --------------------------------------------------------
    // Line 5 - Center frequency
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "FC: %.0f KHZ",
        p.center_hz / 1000.0f
    );

    draw_string(
        0,
        4,
        line
    );


    // --------------------------------------------------------
    // Line 6 - Bandwidth
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "BW: %.0f KHZ",
        p.bandwidth_hz / 1000.0f
    );

    draw_string(
        0,
        5,
        line
    );


    // --------------------------------------------------------
    // Line 7 - Pulse
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "PW: %.0f US",
        p.pulse_us
    );

    draw_string(
        0,
        6,
        line
    );


    // --------------------------------------------------------
    // Line 8 - Waveform
    // --------------------------------------------------------

    snprintf(
        line,
        sizeof(line),
        "MODE: %s",
        sonar_mode_name(p.mode)
    );

    draw_string(
        0,
        7,
        line
    );


    // --------------------------------------------------------
    // Display ON
    // --------------------------------------------------------

    oled_command(0xAF);
}


// ============================================================
// Public initialization
// ============================================================

esp_err_t oled_init()
{
    esp_err_t err =
        oled_i2c_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "I2C initialization failed"
        );

        return err;
    }

    err =
        oled_controller_init();

    if (err != ESP_OK)
    {
        ESP_LOGE(
            TAG,
            "SSD1306 initialization failed"
        );

        return err;
    }

    oled_clear();

    oled_show_startup();

    ESP_LOGI(
        TAG,
        "SSD1306 OLED initialized"
    );

    return ESP_OK;
}