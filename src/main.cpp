#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "SSD1306.h"

int main(int argc, char *argv[])
{
    stdio_init_all();

    SSD1306* ssd = new SSD1306(i2c0, 0x3C, 16, 17);

    ssd->setup(128, 32);

    ssd->drawString(5, 5, "Example Text!");

    ssd->update();

    while (true)
    {
        
        sleep_ms(10);
    }

    delete ssd;

    return 0;
}
