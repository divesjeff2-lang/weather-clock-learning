#include<stdint.h>
#include<string.h>
#include "stm32f4xx.h"
#include "led.h"


int main (void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB ,ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG ,ENABLE);

	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	
	led_init();
	led_all_off();
	
	SysTick->LOAD = SystemCoreClock / 1000;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk  |SysTick_CTRL_ENABLE_Msk |SysTick_CTRL_TICKINT_Msk;
	
	NVIC_SetPriority (SysTick_IRQn ,5);
	
	while(1)
		{
			;
		}
	}
void SysTick_Handler(void)
{	
	 bool led_state = false;
	 int count = 0 ;//data ssd
	
	if (++count >= 500)
	{
		count = 0;
		led_state = !led_state ;
		led_set(1,led_state );
	} 
}
