#include "st7789.h"
#include "font.h"
#include "image.h"

void welcome_page_display(void)
{
    const uint16_t color_background = mkcolor(0, 0, 0);
    st7789_fill_color(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, color_background);
    st7789_write_string(72, 110, "嵌入式", mkcolor(237,128,147), color_background, &font_32_maple_bold);
    st7789_write_string(56, 150, "天气时钟", mkcolor(86,165,255), color_background, &font_32_maple_bold);
    st7789_write_string(60, 240, "Loading...", mkcolor(255, 255, 255), color_background, &font_24_maple);
    // st7789_write_string(20, 300, version, mkcolor(127, 127, 127), color_background, &font_16_maple);
}
