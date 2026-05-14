#include "stm32f4xx.h"


int main (void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC,ENABLE );
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_SYSCFG ,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct .GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_InitStruct .GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct .GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct .GPIO_Speed = GPIO_Medium_Speed;
	GPIO_Init(GPIOB ,&GPIO_InitStruct);
	
	GPIO_WriteBit (GPIOB, GPIO_Pin_0 | GPIO_Pin_1,Bit_SET );
	
	GPIO_InitStruct .GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct .GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct .GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	SYSCFG_EXTILineConfig (EXTI_PortSourceGPIOC, EXTI_PinSource4);
	SYSCFG_EXTILineConfig (EXTI_PortSourceGPIOC, EXTI_PinSource5);
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct .EXTI_Line =EXTI_Line4 | EXTI_Line5;
	EXTI_InitStruct .EXTI_LineCmd = ENABLE;
	EXTI_InitStruct .EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct .EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct );
	
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4 );
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct .NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStruct .NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct .NVIC_IRQChannelPreemptionPriority =5;
	NVIC_InitStruct .NVIC_IRQChannelSubPriority =0;
	NVIC_Init(&NVIC_InitStruct);
	
	while(1)
	{
		
	}
	
}
	void EXTI4_IRQHandler(void)
	{
	if (EXTI_GetFlagStatus (EXTI_Line4 )!=RESET )
	{	
	GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
		EXTI_ClearITPendingBit (EXTI_Line4);
	}	
}	
	
	void EXTI9_5_IRQHandler(void)
	{
	if (EXTI_GetFlagStatus (EXTI_Line5 )!=RESET )
	{	
	GPIO_ToggleBits(GPIOB, GPIO_Pin_1);
		EXTI_ClearITPendingBit (EXTI_Line5);
	}	
}	
	