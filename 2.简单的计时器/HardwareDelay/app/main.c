#include "stm32f4xx.h"

//LED1: PE9
//LED2: PB1
//LED3: PB0

#define LED1_PORT  GPIOE
#define LED1_PIN   GPIO_Pin_9
#define LED2_PORT  GPIOB
#define LED2_PIN 	 GPIO_Pin_1
#define LED3_PORT  GPIOB
#define LED3_PIN   GPIO_Pin_0

void cpu_delay(uint32_t us)
{	
		while(us >= 1000)
		{
			SysTick->LOAD = SystemCoreClock / 1000;
        SysTick->VAL = 0;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
        SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;
        us -= 1000;
		}
		
		if(us >0)
			
		{
			SysTick-> LOAD = us * SystemCoreClock / 1000 / 1000;
			SysTick-> VAL = 0;
			SysTick-> CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk ;
			while((SysTick ->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
			SysTick ->CTRL = SysTick_CTRL_ENABLE_Msk;
		}
}	

		
int main(void)
{
			
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	//// Enable Peripheral Clock for GPIOE & GPIOB on AHB1 Bus
			
			GPIO_InitTypeDef  GPIO_InitStructure;
			GPIO_StructInit(&GPIO_InitStructure);//0
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;//Mode: General Purpose Output
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;//Output Type: Push-Pull
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//Speed: High Speed / Slew Rate
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//Pull-up/Pull-down: Floating / No Pull
	
	
			GPIO_InitStructure.GPIO_Pin = LED1_PIN ;//Configure Port E (PE9)	
			GPIO_Init(LED1_PORT , &GPIO_InitStructure);//Apply configuration to GPIOE Peripheral
	
			while(1)
			{
				for (int i = 0;i<3;i++)
			{
					GPIO_SetBits(LED1_PORT, LED1_PIN);
					cpu_delay(200 * 1000);
					GPIO_ResetBits(LED1_PORT, LED1_PIN);
					cpu_delay(200 * 1000);
			}
				for (int i = 0;i<3;i++)
			{
					GPIO_SetBits(LED1_PORT, LED1_PIN);
					cpu_delay(500 * 1000);
					GPIO_ResetBits(LED1_PORT, LED1_PIN);
					cpu_delay(500 * 1000);
			}
					for (int i = 0;i<3;i++)
			{
					GPIO_SetBits(LED1_PORT, LED1_PIN);
					cpu_delay(200 * 1000);
					GPIO_ResetBits(LED1_PORT, LED1_PIN);
					cpu_delay(200 * 1000);			
			}
			cpu_delay(2000 * 1000);
		}
}
