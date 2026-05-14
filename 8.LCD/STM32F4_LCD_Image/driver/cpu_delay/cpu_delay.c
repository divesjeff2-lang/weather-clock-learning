#include <stdint.h>
#include <string.h> //处理字符串和内存块的工具
#include "stm32f4xx.h"


void cpu_delay(uint32_t us)
{
    while(us >= 1000){
        SysTick->LOAD = SystemCoreClock / 1000;//延迟1毫秒
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0;
        while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);//&称作按位 与操作 只看第16位
        SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;
        us -= 1000;
    }

    if(us > 0){
        SysTick->LOAD = us *(SystemCoreClock / 1000/1000);//注意运算法则  
        //用 1微秒需要跳的次数，乘以你剩下的零头微秒数，就算出了最后要填进 LOAD 旋钮里的总次数
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0;
        while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
        SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;  
    }

}