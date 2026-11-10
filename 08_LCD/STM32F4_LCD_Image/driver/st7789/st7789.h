#ifndef __ST7789_H__
#define __ST7789_H__

#include <stdbool.h>
#include <stdint.h>
#include "font.h"
#include "image.h"


#define ST7789_WIDTH   240
#define ST7789_HEIGHT  320
#define mkcolor(r,g,b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))
//RGB565 16位
//red 5位  green 6位  blue 5位  uint16_t
//将红色清零后三位 左移八位  //g先清零后2位 左移三位 推到中间  //b右位三位 丢后3位
//xxxx xxxx  
//1111 1000                 //1111 1100                     //
//xxxx x000  xxxx xx00  xxxx x
//xxxx xxxx xxxx xxxx       //拼接完整代码
void st7789_init(void);
void st7789_fill_color(uint16_t x, uint16_t y ,uint16_t width, uint16_t height, uint16_t color);
void st7789_write_string(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, const font_t *font);
void st7789_draw_image(uint16_t x, uint16_t y, const image_t *image);


#endif /* __ST7789_H__ */
