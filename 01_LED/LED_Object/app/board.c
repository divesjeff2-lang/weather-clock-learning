#include "board.h"
#include "led_desc.h"

void board_lowlevel_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
}

static struct led_desc _led1 = { GPIOE, GPIO_Pin_9, Bit_RESET, Bit_SET};
static struct led_desc _led2 = { GPIOB, GPIO_Pin_0, Bit_RESET, Bit_SET};
static struct led_desc _led3 = { GPIOB, GPIO_Pin_1, Bit_RESET, Bit_SET};
led_desc_t led1 = &_led1;
led_desc_t led2 = &_led2;
led_desc_t led3 = &_led3;
