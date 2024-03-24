#include "SSD1306.h"

SSD1306::SSD1306(i2c_inst_t *port, uint8_t deviceAddr, int sda_pin, int scl_pin)
{
    // Set up I2C communication
    m_i2c_port = port;
    m_device_addr = deviceAddr;
    m_sda_pin = sda_pin;
    m_scl_pin = scl_pin;

    i2c_init(m_i2c_port, 400000);

    gpio_set_function(m_sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(m_scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(m_sda_pin);
    gpio_pull_up(m_scl_pin);
}

void SSD1306::setup(uint16_t width, uint16_t height)
{
    // Give time for board to start up
    sleep_ms(500);

    m_width = width;
    m_height = height;
    m_pages = height / 8;

    // Gives us a byte per pixel for buffer
    m_buffer_size = (m_pages) * (width);

    m_disp_buffer = new uint8_t[m_buffer_size + 1];

    // This might cause problems later, from the examples
    //  I was looking at it had this line but it doesn't seem
    //  to break the program with or without so I'll keep it in
    //  for now
    ++(m_disp_buffer);

    // from https://github.com/makerportal/rpi-pico-ssd1306
    // and https://github.com/daschr/pico-ssd1306
    uint8_t cmds[] = {
        SET_DISP,
        // timing and driving scheme
        SET_DISP_CLK_DIV,
        0x80,
        SET_MUX_RATIO,
        height - 1,
        SET_DISP_OFFSET,
        0x00,
        // resolution and layout
        SET_DISP_START_LINE,
        // charge pump
        SET_CHARGE_PUMP,
        m_external_vcc ? 0x10 : 0x14,
        SET_SEG_REMAP | 0x01,   // column addr 127 mapped to SEG0
        SET_COM_OUT_DIR | 0x08, // scan from COM[N] to COM0
        SET_COM_PIN_CFG,
        width > 2 * height ? 0x02 : 0x12,
        // display
        SET_CONTRAST,
        0xff,
        SET_PRECHARGE,
        m_external_vcc ? 0x22 : 0xF1,
        SET_VCOM_DESEL,
        0x30,          // or 0x40?
        SET_ENTIRE_ON, // output follows RAM contents
        SET_NORM_INV,  // not inverted
        SET_DISP | 0x01,
        // address setting
        SET_MEM_ADDR,
        0x00, // horizontal
    };

    // Initialization commands
    for (size_t i = 0; i < sizeof(cmds); ++i)
        sendCommand(cmds[i]);

    clearBuffer();
    update();
}

SSD1306::~SSD1306()
{
    // Cleanup
    delete m_disp_buffer;
}

int SSD1306::sendCommand(uint8_t command)
{
    // I have no idea why we send a 0x00 first but it works
    // I THINK 0x00 is the write address for the chip
    uint8_t data[2] = {0x00, command};
    return i2c_write_blocking(m_i2c_port, m_device_addr, data, 2, false);
}

void SSD1306::clearBuffer()
{
    // In case of error: memset(m_disp_buffer, 0, m_buffer_size);
    std::fill_n(m_disp_buffer, m_buffer_size, 0);
}

void SSD1306::drawPixel(uint32_t x, uint32_t y, uint8_t state)
{
    // Does nothing if pixel is out of bounds
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 32)) 
    {
        return;
    }

    if (state == 1) 
    {
        // Turns a pixel on
        m_disp_buffer[x + (y / 8)*128] |= (1 << (y & 7));
    } else 
    {
        // Turns a pixel off
        m_disp_buffer[x + (y / 8)*128] &= ~(1 << (y & 7));
    }
}

// As far as I know this will only work with the font with repo
void SSD1306::drawChar(uint32_t x, uint32_t y, char c)
{
    for (size_t i = 0; i <= 4; i++)
    {
        for (size_t k = 0; k <= 7; k++)
        {
            uint8_t res = ((ASCII[c - 0x20][i]) >> k) & 1;

            if (res == 1)
            {
                drawPixel(x + i, y + k, 1);
            } else
            {
                drawPixel(x + i, y + k, 0);
            }
        }
    }
}

void SSD1306::drawString(uint32_t x, uint32_t y, const char * str)
{
    // make count and offset ints if issues occur
    size_t count = 0;
    uint16_t offset = 0;

    while (str[count] != '\0')
    {
        drawChar(x + offset, y, str[count]);

        offset += 8;
        count++;
    }
}

void SSD1306::clearPixel(uint32_t x, uint32_t y)
{
    m_disp_buffer[x + m_width * (y >> 3)] &= 0x0 << (y & 0x07);
}

void SSD1306::update()
{
    uint8_t payload[] = {SET_COL_ADDR, 0, m_width - 1, SET_PAGE_ADDR, 0, m_pages - 1};
    if (m_width == 64)
    {
        payload[1] += 32;
        payload[2] += 32;
    }

    for (size_t i = 0; i < sizeof(payload); ++i)
        sendCommand(payload[i]);

    *(m_disp_buffer - 1) = 0x40;

    i2c_write_blocking(m_i2c_port, m_device_addr, m_disp_buffer - 1, m_buffer_size + 1, false);
}