#pragma once

#include <iostream>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "font.h"

#include "string.h"
#include <algorithm>

// SSD1306 Commands
typedef enum {
    SET_CONTRAST = 0x81,
    SET_ENTIRE_ON = 0xA4,
    SET_NORM_INV = 0xA6,
    SET_DISP = 0xAE,
    SET_MEM_ADDR = 0x20,
    SET_COL_ADDR = 0x21,
    SET_PAGE_ADDR = 0x22,
    SET_DISP_START_LINE = 0x40,
    SET_SEG_REMAP = 0xA0,
    SET_MUX_RATIO = 0xA8,
    SET_COM_OUT_DIR = 0xC0,
    SET_DISP_OFFSET = 0xD3,
    SET_COM_PIN_CFG = 0xDA,
    SET_DISP_CLK_DIV = 0xD5,
    SET_PRECHARGE = 0xD9,
    SET_VCOM_DESEL = 0xDB,
    SET_CHARGE_PUMP = 0x8D
} ssd1306_command_t;

class SSD1306
{
    private:
        // Should be either 0x3C or 0x3D depending on chip
        uint8_t m_device_addr;

        // I2C port
        i2c_inst_t* m_i2c_port;

        // I2C pins
        int m_sda_pin;
        int m_scl_pin;

        bool m_external_vcc = false;

        // Display information
        uint8_t * m_disp_buffer;
        size_t m_buffer_size = 0;
        uint16_t m_width = 0;
        uint16_t m_height = 0;
        uint16_t m_pages = 0;

        // Sends a command to the chip 
        // Returns bytes transmitted if successful; -1 if fails
        int sendCommand(uint8_t command);
    public:
        SSD1306(i2c_inst_t * port, uint8_t deviceAddr, int sda_pin, int scl_pin);
        ~SSD1306();

        // Function for SSD1306 initialization
        // Must call this to initialize properly
        void setup(uint16_t width, uint16_t height);

        // Updates display
        void update();

        // Draws a pixel at the given coordinates
        void drawPixel(uint32_t x, uint32_t y, uint8_t state);

        // Draws a character at the given coordinates
        void drawChar(uint32_t x, uint32_t y, char c);

        // Draws a set of characters at the given coordinates
        void drawString(uint32_t x, uint32_t y, const char * str);

        // Turns a pixel off at the given coordinates
        void clearPixel(uint32_t x, uint32_t y);

        // Clears the display buffer
        void clearBuffer();
};