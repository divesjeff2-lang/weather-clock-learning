#ifndef __KEY_H__
#define __KEY_H__

#include <stdbool.h>
#include <stdint.h>

void key_init(void);
bool key_read(uint8_t idx);

#endif /* __KEY_H__ */
