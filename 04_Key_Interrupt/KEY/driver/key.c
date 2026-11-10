#include <stdbool.h>
#include "stm32f4xx.h"

// KEY1: PA0
// KEY2: PC4
// KEY3: PC5

#define KEY1_PORT   GPIOA
#define KEY1_PIN    GPIO_Pin_0
#define KEY2_PORT   GPIOC
#define KEY2_PIN    GPIO_Pin_4
#define KEY3_PORT   GPIOC
#define KEY3_PIN    GPIO_Pin_5

void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Medium_Speed;
    
    GPIO_InitStructure.GPIO_Pin = KEY1_PIN;
    GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY2_PIN;
    GPIO_Init(KEY2_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = KEY3_PIN;
    GPIO_Init(KEY3_PORT, &GPIO_InitStructure);
}

bool key_read(uint8_t idx)
{
    switch (idx)
    {
        case 1:  return GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == Bit_SET;
        case 2:  return GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == Bit_SET;
        case 3:  return GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == Bit_SET;
        default: return false;
    }
}
