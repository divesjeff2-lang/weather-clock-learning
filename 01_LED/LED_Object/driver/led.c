#include <stdbool.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "led.h"
#include "led_desc.h"

void led_init(led_desc_t led)
{
			GPIO_InitTypeDef  GPIO_InitStructure;
			GPIO_StructInit(&GPIO_InitStructure);//0
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;//Mode: General Purpose Output
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;//Output Type: Push-Pull
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//Speed: High Speed / Slew Rate
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//Pull-up/Pull-down: Floating / No Pull
			GPIO_InitStructure.GPIO_Pin = led->Pin ;//Configure Port E (PE9)	
			GPIO_Init(led->Port , &GPIO_InitStructure);//Apply configuration to GPIOE Peripheral
}

	void led_set(led_desc_t led, bool onoff)
	{
		GPIO_WriteBit(led->Port, led->Pin, onoff ? led->OnBit : led->OffBit);
	}
	
	void led_on(led_desc_t led)
	{
		GPIO_WriteBit(led->Port, led->Pin, led->OnBit);
	}
	
		void led_off(led_desc_t led)
	{
		GPIO_WriteBit(led->Port, led->Pin, led->OffBit);
	}
		