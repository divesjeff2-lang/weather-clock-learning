#include <stdint.h>// C语言标准库 <> compiler(编译器)去系统默认的库文件search 
#include "stm32f4xx.h"//"" go priorly to search project of yourself folder 
#include "st7789.h"                   //cosnt 常量死死锁在FLASH里面 不会再ram里面乱跑
#include "font.h"

int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    st7789_init();
	
		//st7789_fill_color(0, 0, 79, 319, mkcolor(255, 0, 0));
    //st7789_fill_color(80, 0, 159, 319, mkcolor(0, 255, 0));
    //st7789_fill_color(160, 0, 239, 319, mkcolor(0, 0, 255));
	

    	// 先把全屏刷成黑色，作为干净的黑板
    st7789_fill_color(0, 0, 239, 319, mkcolor(0, 0, 0));

    st7789_write_string(40,150,"Hello World!",mkcolor(255,255,255),mkcolor(0,0,0), &font16);
    
    st7789_write_string(10, 100, "嵌入式", mkcolor(255,255,0), mkcolor(0,0,0), &font16);
    while(1)
    {
			
    }
}
