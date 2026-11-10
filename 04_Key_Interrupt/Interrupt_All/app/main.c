#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "cpu_delay.h"

#define delay(ms) cpu_delay((ms) * 1000)

typedef void (*func)(void);

static void led1_toggle(void)
{
    static bool state = false;
    led_set(1, state);
    state =!state;
    led_set(1, state);
}
static void led2_toggle(void)
{
  	static bool state = false;
  	led_set(2, state);
  	state =!state;
  	led_set(2, state);
}
static void led3_toggle(void)
{
  	static bool state = false;
  	led_set(3, state);
  	state =!state;
  	led_set(3, state);
}

static void usart_received_func(uint8_t data)
{
	switch(data)
	{
		case '1': led1_toggle ();break;
		case '2': led2_toggle ();break;
		case '3': led3_toggle ();break;
		default	:	break;
	}
}
	int main(void)
{

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,  ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,  ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,  ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		led_init() ;
    key_init();
		key_press_callback_register(1,  led1_toggle );	
		key_press_callback_register(2,  led2_toggle );	
		key_press_callback_register(3,  led3_toggle );	
    usart_init();
		usart_received_register (usart_received_func);
	
    while(1)
  {
			;
	}
}	
