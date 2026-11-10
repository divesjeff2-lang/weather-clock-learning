#ifndef __FONT_H__
#define __FONT_H__

#include <stdint.h>

typedef struct
{
    const char *name;
    const uint8_t *model;
} font_chinese_t;

typedef struct
{
    uint16_t size;
    const uint8_t *ascii_model;
    const font_chinese_t *chinese;
} font_t;

extern const font_t font16;
extern const font_t font32;
extern const font_t font48;

#endif /* __FONT_H__ */
