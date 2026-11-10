#include <stdint.h>
#include "board.h"
#include "cpu_delay.h"
#include "stm32f4xx.h"

int main(void)
{
    board_lowlevel_init();
	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_StructInit (&GPIO_InitStruct );
		GPIO_InitStruct .GPIO_Mode = GPIO_Mode_AF ;
		GPIO_InitStruct .GPIO_OType = GPIO_OType_PP ;
		GPIO_InitStruct .GPIO_PuPd = GPIO_PuPd_NOPULL ;
		GPIO_InitStruct .GPIO_Pin = GPIO_Pin_9; 
		GPIO_Init(GPIOE,&GPIO_InitStruct);
		GPIO_InitStruct .GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
		GPIO_PinAFConfig (GPIOE,GPIO_PinSource9 ,GPIO_AF_TIM1 );
	  GPIO_PinAFConfig (GPIOB,GPIO_PinSource0 ,GPIO_AF_TIM3 );
	  GPIO_PinAFConfig (GPIOB,GPIO_PinSource1 ,GPIO_AF_TIM3 );
	
		RCC_ClocksTypeDef RCC_ClocksStruct;
		RCC_GetClocksFreq (&RCC_ClocksStruct);
		uint32_t apb2_timclk_mhz = RCC_ClocksStruct .PCLK2_Frequency /1000/1000* 2;
		uint32_t apb1_timclk_mhz = RCC_ClocksStruct .PCLK1_Frequency /1000/1000* 2;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;
		TIM_TimeBaseInitStruct.TIM_Prescaler = apb2_timclk_mhz - 1;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);
		TIM_TimeBaseInitStruct.TIM_Prescaler = apb1_timclk_mhz - 1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
		
		TIM_OCInitTypeDef TIM_OCInitStruct;
		TIM_OCStructInit(&TIM_OCInitStruct);
		TIM_OCInitStruct .TIM_OCMode = TIM_OCMode_PWM1 ;
		TIM_OCInitStruct .TIM_OutputState = TIM_OutputState_Enable ;
		TIM_OCInitStruct .TIM_Pulse = 500-1;
		TIM_OCInitStruct .TIM_OCPolarity = TIM_OCPolarity_High ;
		TIM_OC1Init(TIM1,&TIM_OCInitStruct);
		TIM_OCInitStruct .TIM_OCPolarity = TIM_OCPolarity_High ;
		TIM_OC3Init(TIM3,&TIM_OCInitStruct);
		TIM_OC4Init(TIM3,&TIM_OCInitStruct);
		
		TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable); 
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); 
		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); 
		TIM_CtrlPWMOutputs(TIM1, ENABLE);
   
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
		
		uint32_t led1_pwm_duty = 500-1 ,led2_pwm_duty = 500-1 ,led3_pwm_duty = 500-1;
		int led1_pwm_duty_inc = 1,led2_pwm_duty_inc = 1,led3_pwm_duty_inc = 1;
		
		int led_pwm_duty = 500-1;
		int led_pwm_duty_inc = 1;
    while(1)
    {
			cpu_delay(1000);
			
			led1_pwm_duty += led1_pwm_duty_inc ;
			if (led_pwm_duty >= 1000-1 || led_pwm_duty <= 0)
				led_pwm_duty_inc = - led_pwm_duty_inc ;
				
			TIM_SetCompare1 (TIM1,led1_pwm_duty);
    	TIM_SetCompare3 (TIM3,led1_pwm_duty);
    	TIM_SetCompare4 (TIM3,led1_pwm_duty);


			;
    }
}
