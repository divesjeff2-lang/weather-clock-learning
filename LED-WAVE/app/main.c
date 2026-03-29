#include<stdbool.h>
#include "stm32f4xx.h"
#include "led.h"

void led_breath(int duty,int time)
{						
		for (int t = 0; t <time; t++)
						{				
							led_on(1);
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
			
			int led1_duty = 30,led1_inc=(1);
			int led2_duty = 40,led2_inc=(1);
			int led3_duty = 50,led3_inc=(1);
	;
			while(1)
			{	
				
				for(int t=0;t<20;t++)
					{
					for(int i = 0;i<600 ;i++)
							{
					if(i< led1_duty) led_on(1);else led_off(1);
					if(i< led2_duty) led_on(2);else led_off(2);
					if(i< led3_duty) led_on(3);else led_off(3);
				}
			}
					led1_duty += led1_inc;if(led1_duty >=60 ||led1_duty < 0) led1_duty= -led1_inc;
					led2_duty += led2_inc;if(led2_duty >=60 ||led2_duty < 0) led2_duty= -led1_inc;
					led3_duty += led3_inc;if(led3_duty >=60 ||led3_duty < 0) led3_duty= -led1_inc;
		}
	}		
