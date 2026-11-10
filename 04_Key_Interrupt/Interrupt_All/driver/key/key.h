#ifndef __KEY_H__
#define __KEY_H__

#include <stdbool.h>
#include <stdint.h>

typedef void (*key_func_t)(void);

void key_init(void);
bool key_read(uint8_t idx);
void key_press_callback_register(uint8_t kidx, key_func_t func);


#endif /* __KEY_H__ */
