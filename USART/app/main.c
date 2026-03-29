#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"

static void board_lowlevel_init(void)
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1ENR_GPIOAEN,ENABLE);
	RCC_APB2PeriphClockCmd (RCC_APB2ENR_USART1EN,ENABLE);
}

static void usart_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_StructInit (&USART_InitStructure );
 
	USART_InitStructure .USART_BaudRate = 19200u;
	USART_InitStructure .USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure .USART_Mode =USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure .USART_Parity = USART_Parity_No;
	USART_InitStructure .USART_StopBits = USART_StopBits_1;
	USART_InitStructure .USART_WordLength =USART_WordLength_8b;
	USART_Init  (USART1, &USART_InitStructure );
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_USART1);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure );
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_Init  (USART1, &USART_InitStructure );
  USART_Cmd(USART1, ENABLE);
}

static void usart_write(const char str[])
{
		int len = strlen(str);
		for (int i= 0; i< strlen(str);i++)
	{
		USART_ClearFlag (USART1 ,USART_FLAG_TC );
		USART_SendData (USART1 ,str[i]);
		while ( USART_GetFlagStatus (USART1, USART_FLAG_TC) == RESET);
	}
	}

 int main(void)
 {
		board_lowlevel_init();
		usart_init ();
	 
	 usart_write("hello world\r\n");
	 printf("hello world\r\n");
	
	 
	 while(1)
	 {
		  USART_ClearFlag (USART1 ,USART_FLAG_RXNE  );
			while ( USART_GetFlagStatus (USART1, USART_FLAG_RXNE) == RESET);
		 
		  uint8_t rdata = (uint8_t)USART_ReceiveData (USART1 );
		 
		  printf("received:%c ascii:%d\r\n",rdata,rdata);
	 }
	}


	int fputc(int c,FILE *stream)
	{
		(void)stream;
		
			USART_ClearFlag (USART1 ,USART_FLAG_TC );
			USART_SendData (USART1 ,(uint16_t)c);
			while ( USART_GetFlagStatus (USART1, USART_FLAG_TC) == RESET);
		
		return c;

	}
	