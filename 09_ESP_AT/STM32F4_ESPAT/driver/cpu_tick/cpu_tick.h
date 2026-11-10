#ifndef __CPU_DELAY_H__
#define __CPU_DELAY_H__

#include <stdint.h>

void cpu_tick_init(void);
uint64_t cpu_now(void);
uint64_t cpu_get_us(void);
uint64_t cpu_get_ms(void);
void cpu_delay_us(uint32_t us);
void cpu_delay_ms(uint32_t ms);

#endif /* __CPU_DELAY_H__ */
