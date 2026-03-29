#ifndef __LED_H__
#define __LED_H__

#include <stdbool.h>
#include <stdint.h>

void led_init(void);
void led_set(uint8_t idx, bool onoff);
void led_on(uint8_t idx);
void led_off(uint8_t idx);
void led_all_off(void);

#endif /* __LED_H__ */
