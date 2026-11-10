#include <stdint.h>
#include "board.h"
#include "stm32f4xx.h"

int main(void)
{
	board_lowlevel_init();
	led_init (led1);
	led_init (led2);
	led_init (led3);
	
	NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4 );
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct .TIM_Prescaler = 84-1;
	TIM_TimeBaseInitStruct .TIM_CounterMode =TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct .TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInitStruct .TIM_Period = 1000-1;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	NVIC_InitTypeDef  NVIC_InitStruct;
	NVIC_InitStruct .NVIC_IRQChannel =TIM6_DAC_IRQn;
	NVIC_InitStruct .NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct .NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStruct .NVIC_IRQChannelSubPriority =0;
	NVIC_Init (&NVIC_InitStruct );
	
	TIM_ITConfig (TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd (TIM6,ENABLE);
	
  while(1)
		{;}
}

void TIM6_DAC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

        static int count = 0;
        static bool state = false;
        if (++count >= 500)
        {
            count = 0;
            state = !state;
            led_set(led1, state);
        }
    }
}
