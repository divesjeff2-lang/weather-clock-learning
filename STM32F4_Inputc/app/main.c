#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "stm32f4xx.h"
#include "cpu_delay.h"
#include "usart.h"

static volatile uint32_t duty_cycle;
static volatile uint32_t frequency;

int main(void)  
{
    board_lowlevel_init();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    usart_init();
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
    
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
    
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
    TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);

    TIM_Cmd(TIM3, ENABLE);

    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

    char str[100];
    while (1)
    {
        cpu_delay(100ul * 1000ul);
        snprintf(str, sizeof(str), "freq: %lu, duty: %lu\n", frequency, duty_cycle);
        usart_write(str);
    }
}

void TIM3_IRQHandler(void)
{
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq(&RCC_Clocks);
    uint32_t tim3_apb1_clk_hz = RCC_Clocks.PCLK1_Frequency * 2ul;

    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

    uint32_t ic1 = TIM_GetCapture1(TIM3) + 1;
    uint32_t ic2 = TIM_GetCapture2(TIM3) + 1;

    if (ic1 != 0)
    {
        duty_cycle = (ic2 * 100) / ic1;
        frequency = tim3_apb1_clk_hz / ic1;
    }
    else
    {
        duty_cycle = 0;
        frequency = 0;
    }
}
