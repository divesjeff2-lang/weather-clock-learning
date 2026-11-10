#include <stdint.h>
#include "stm32f4xx.h"
#include "../driver/st7789/st7789.h"

int main(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // Enable GPIOB clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); // Enable GPIOC clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // Enable SPI2 clock

	st7789_init();

	st7789_fill_color(0, 0, ST7789_WIDTH, ST7789_HEIGHT, mkcolor(255, 0, 0)); // Fill screen with red color
	st7789_fill_color(0, 0, ST7789_WIDTH, ST7789_HEIGHT, mkcolor(0, 255, 0)); // Fill screen with green color
	st7789_fill_color(0, 0, ST7789_WIDTH, ST7789_HEIGHT, mkcolor(0, 0, 255)); // Fill screen with blue color


	while(1)
	{
		;
	}


}