#include <stdint.h>
#include "stm32f4xx.h"
#include "st7789.h"

int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    st7789_fill_color(0, 0, 79, 319, mkcolor(255, 0, 0));
    st7789_fill_color(80, 0, 159, 319, mkcolor(0, 255, 0));
    st7789_fill_color(160, 0, 239, 319, mkcolor(0, 0, 255));
    
    while (1)
    {
        ;
    }
}
