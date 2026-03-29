#include <stdbool.h>
#include <stddef.h>
#include "stm32f4xx.h"
#include "key.h"

// KEY1: PA0
// KEY2: PC4
// KEY3: PC5

struct key_inst
{
    GPIO_TypeDef    *port;
    uint16_t        pin;
    uint8_t         exti_port_src;
    uint8_t         exti_pin_src;
    uint32_t        exti_line;
    uint8_t         irqn;
    key_func_t      func;
};

////struct key_inst key0 =
////{
////    .port = GPIOA,
////};

#define KEY_DEFINE(n,PORT ,PIN,IRQn) \
static  struct key_inst key##n = \
{ \
            .   port = GPIO##PORT, \
            .   pin = GPIO_Pin_##PIN,\
            .   exti_port_src = EXTI_PortSourceGPIO##PORT, \
            .   exti_pin_src = EXTI_PinSource##PIN, \
            .   exti_line =EXTI_Line##PIN, \
            .   irqn = IRQn,\
}

KEY_DEFINE (1, A, 0, EXTI0_IRQn);
KEY_DEFINE (2, C, 4, EXTI4_IRQn);
KEY_DEFINE (3, C, 5, EXTI9_5_IRQn);

//static void key_lowlevel_init(void)
//{
//      
//}
//static void key_irq_init(void)
//{
//      
//}

void key_init(void)
{
        //GPIO初始化结构体模板
    GPIO_InitTypeDef        GPIO_InitStructure;
    GPIO_StructInit         (&GPIO_InitStructure);
    GPIO_InitStructure  .GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure  .GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStructure  .GPIO_Speed = GPIO_Medium_Speed;
    
        //EXTI中断初始化结构体模板
        EXTI_InitTypeDef    (EXTI_InitStructure);
        EXTI_StructInit     (&EXTI_InitStructure);
        EXTI_InitStructure .EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure .EXTI_Trigger = EXTI_Trigger_Rising ;
        EXTI_InitStructure .EXTI_LineCmd = ENABLE;
    
        //EXTI中断优先级配置结构体模板
        NVIC_InitTypeDef        NVIC_InitStructure;
        NVIC_InitStructure .NVIC_IRQChannelPreemptionPriority = 5;
        NVIC_InitStructure .NVIC_IRQChannelSubPriority =0;
        NVIC_InitStructure .NVIC_IRQChannelCmd =ENABLE;
        
        //KEY1初始化
    GPIO_InitStructure  .GPIO_Pin = key1.pin ;
    GPIO_Init(key1.port , &GPIO_InitStructure);
        SYSCFG_EXTILineConfig(key1.exti_port_src,key1.exti_pin_src);
        EXTI_InitStructure  .EXTI_Line = key1.exti_line ;
        EXTI_Init(&EXTI_InitStructure);
        NVIC_InitStructure  .NVIC_IRQChannel = key1.irqn;
        NVIC_Init(&NVIC_InitStructure); 
        
    //KEY2初始化
    GPIO_InitStructure  .GPIO_Pin = key2.pin ;
    GPIO_Init(key2.port , &GPIO_InitStructure);
        SYSCFG_EXTILineConfig(key2.exti_port_src,key2.exti_pin_src);
        EXTI_InitStructure  .EXTI_Line = key2.exti_line ;
        EXTI_Init(&EXTI_InitStructure);
        NVIC_InitStructure  .NVIC_IRQChannel = key2.irqn;
        NVIC_Init(&NVIC_InitStructure); 
        
        //KEY3初始化
    GPIO_InitStructure  .GPIO_Pin = key3.pin ;
    GPIO_Init(key3.port , &GPIO_InitStructure);
        SYSCFG_EXTILineConfig(key3.exti_port_src,key3.exti_pin_src);
        EXTI_InitStructure  .EXTI_Line = key3.exti_line ;
        EXTI_Init(&EXTI_InitStructure);
        NVIC_InitStructure  .NVIC_IRQChannel = key3.irqn;
        NVIC_Init(&NVIC_InitStructure); 
}

bool key_read(uint8_t idx)
{
    switch (idx)
    {
            case 1:  return GPIO_ReadInputDataBit(key1.port, key1.pin) == Bit_SET;
            case 2:  return GPIO_ReadInputDataBit(key2.port, key2.pin) == Bit_SET;
            case 3:  return GPIO_ReadInputDataBit(key3.port, key3.pin) == Bit_SET;
            default: return false;
    }
}

void key_press_callback_register(uint8_t kidx, key_func_t func)
{
        switch (kidx)
        {
            case 1  : key1.func = func; break;
            case 2  : key2.func = func; break;
            case 3  : key3.func = func; break;
            default : break;
        }
}

void EXTI0_IRQHandler(void)
{
         EXTI_ClearITPendingBit (EXTI_Line0  );
         if (key1.func != NULL)
         {
                key1.func ();
         }
         
}
void EXTI4_IRQHandler(void)
{
         EXTI_ClearITPendingBit (EXTI_Line4  );
            if (key2.func != NULL)
         {
                key2.func ();
         }
}
void EXTI9_5_IRQHandler(void)
{
         EXTI_ClearITPendingBit (EXTI_Line5  );
            if (key3.func != NULL)
         {
                key3.func ();
         }
     }