#include <stdbool.h>
#include <stdint.h>
#include "board.h"

static void led_delay(void)
{
    for (int i = 0; i < 500000; i++)
    {
        __nop();
    }
}

int main(void)
{
			board_lowlevel_init();
	
			led_init(led1);
			led_init(led2);
			led_init(led3);
	
			while(1)
			{
				led_on(led1); led_off(led2); led_on(led3);
				led_delay();
				
				led_off(led1); led_on(led2); led_off(led3);
				led_delay();
			}
}
