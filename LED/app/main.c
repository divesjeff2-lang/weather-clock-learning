#include "stm32f4xx.h"

int main(void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB ,ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE ,ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct .GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct .GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct .GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct .GPIO_Speed  = GPIO_High_Speed;
	GPIO_Init(GPIOE,&GPIO_InitStruct );
	
	while(1)
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_9);
		GPIO_ResetBits(GPIOE,GPIO_Pin_9);
	}
}