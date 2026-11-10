#include<stdbool.h>
#include "stm32f4xx.h"
#include "led.h"

void led_breath(int duty,int time)
{						
		for (int t = 0; t < 3000; t++)
						{				led_on(1);
										for(int i = 0; i < duty; i++);
										led_off(1);
										for(int i = duty; i < 100; i++);
						}
}
		int main(void)	
{
			
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//// Enable Peripheral Clock for GPIOE & GPIOB on AHB1 Bus
			
			led_init();
			led_all_off();
	
			while(1)
			{	
							
								int  period= 100;
								for (int d = 0; d < period; d++)
								{	
										
										led_breath(d,3000);
								for (int d = period ; d > 0 ; d--)
								{
								
									led_breath(d,3000);
								}
				
				
				}
}



}
