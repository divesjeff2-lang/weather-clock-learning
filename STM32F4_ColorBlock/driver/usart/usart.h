#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

typedef void (*usart_received_func_t)(uint8_t data);

void usart_init(void);
void usart_received_register(usart_received_func_t func);
void usart_write(const char str[]);

#endif /* __USART_H__ */
