#include <stdint.h>
#include "stm32f4xx.h"
#include "cpu_delay.h"

#define CS_PORT     GPIOE
#define CS_PIN      GPIO_Pin_2
#define RESET_PORT  GPIOE
#define RESET_PIN   GPIO_Pin_3
#define DC_PORT     GPIOE
#define DC_PIN      GPIO_Pin_4
#define BL_PORT     GPIOE
#define BL_PIN      GPIO_Pin_5



//1.铺设物理线路 配置控制引脚 和通讯协议 (GPIO SPI)：
void st7789_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_StructInit  (&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOE,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2,GPIO_AF_SPI2);

	SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit (&SPI_InitStruct);

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_InitStruct;

	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);

	st7789_init_display();
}


//2.打造对讲机（编写基础读写函数）
static void st7789_write_register(uint8_t reg , uint8_t data[], uint8_t length)
{
	GPIO_ResetBits(CS_PORT,CS_PIN);
	GPIO_ResetBits(DC_PORT,DC_PIN);

	SPI_SendData(SPI2, reg);
	while(SPI_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET);

	GPIO_SetBits(DC_PORT, DC_PIN);
	for(uint16_t i = 0; i < length; i++)
	{
		SPI_SendData(SPI2,data[i]);
		while(SPI_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE ) == RESET);
	}

	while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
	GPIO_SetBits(CS_PORT,CS_PIN);
//接通电话 -> 切指令频道发指令 -> 切数据频道发数据 -> 挂电话。
} 

//3.硬件重启
static void st7789_reset(void)
{
	GPIO_ResetBits(RESET_PORT,RESET_PIN);
	delay_us(20);
	GPIO_SetBits(RESET_PORT,RESET_PIN);
	delay_us(120);
}

//4.调用运输车，给屏幕发 0x11、0x36 等开机魔咒，把屏幕唤醒，告诉它像素怎么排布。
static void st7789_init_display(void)
{
    st7789_reset();
    
    st7789_write_register(0x11, NULL, 0);
    delay_ms(5);

    //reg data length
    st7789_write_register(0x36, (uint8_t[]){0x00}, 1);
    st7789_write_register(0x3A, (uint8_t[]){0x55}, 1);
    st7789_write_register(0xB7, (uint8_t[]){0x46}, 1);
    st7789_write_register(0xBB, (uint8_t[]){0x1B}, 1);
    st7789_write_register(0xC0, (uint8_t[]){0x2C}, 1);
    st7789_write_register(0xC2, (uint8_t[]){0x01}, 1);
    st7789_write_register(0xC4, (uint8_t[]){0x20}, 1);
    st7789_write_register(0xC6, (uint8_t[]){0x0F}, 1);
    st7789_write_register(0xD0, (uint8_t[]){0xA4,0xA1}, 2);
    st7789_write_register(0xD6, (uint8_t[]){0xA1}, 1);
    st7789_write_register(0xE0, (uint8_t[]){0xF0,0x00,0x06,0x04,0x05,0x05,0x31,0x44,0x48,0x36,0x12,0x12,0x2B,0x34}, 14);
    st7789_write_register(0xE0, (uint8_t[]){0xF0,0x0B,0x0F,0x0F,0x0D,0x26,0x31,0x43,0x47,0x38,0x14,0x14,0x2C,0x32}, 14);
    st7789_write_register(0x21, NULL, 0);
    st7789_write_register(0x29, NULL, 0);

  	st7789_fill_color(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1, 0x0000);
    st7789_set_backlight(true);
}

//5.发数据前的保安，检查你要画的区域超没超出 240x320 的物理背板
static bool in_screen_range(uint16_t x1, uint16_t y1,uint16_t x2, uint16_t y2)
{
	if(x1 >= ST7789_WIDTH) || (y1 >= ST7789_HEIGHT)  return false;
	if(x2 >= ST7789_WIDTH) || (y2 >= ST7789_HEIGHT)  return false;
	if(x1 > x2 || y1 > y2)
	return true;

}

//6.用运输车发 0x2A/0x2B，在显存里圈出一块特定的长方形地皮。
static void st7789_set_range(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    st7789_write_register(0x2A,(uint8_t[]) {(x1 >> 8) & 0xff, x1 & 0xff, (x2 >> 8 ) & 0xff, x2 & 0xff} ,4);
    //0x2A指令: 设置列地址 x范围 也就是宽
    //Y坐标最大 319 转换二进制为 0000 0001 0011 1111 在二进制为16为数字
    //SPI一次最多8个数字 所以16位要砍半发送
    st7789_write_register(0x2B,(uint8_t[]) {(y1 >> 8) & 0xff, y1 & 0xff, (y2 >> 8 ) & 0xff, y2 & 0xff} ,4);
    //0x2B指令: 设置列地址 y范围 也就是高
}
static void st7789_set_gram_mode(void)
{
    st7789_write_register(0x2C, NULL, 0);
}

//7.送纯色数据。把同一个颜色通过 SPI 运输车倒进地皮。
void st7789_fill_color(uint16_t x1 , uint16_t y1 ,uint16_t x2, uint16_t y2,uint16_t color)
{
    if(!in_screen_range(x1,y1,x2,y2))
        return;
    
    st7789_set_range(x1,y1,x2,y2);//range
    st7789_set_gram_mode();//screen data

    uint8_t color_data[2] = {(color >> 8 ) & 0xff, color & 0xff};//SPI发报机 只能发送8位字节 传递颜色16位时候 只能分两次寄送
    uint32_t pixels = (x2 - x1 + 1) * (y2 - y1 + 1);

    GPIO_ResetBits(CS_PORT,CS_PIN);//拉低电平 接受信号
    GPIO_SetBits(DC_PORT,DC_PIN);//DC=1

    for(uint32_t i =0; i< pixels; i++)
    {
        SPI_SendData(SPI2, color_data[0]);//分两段寄送
        while(SPI_GetFlagStatus(SPI2,SPI_FLAG_TXE) == RESET);//等发件箱为空
        SPI_SendData(SPI2, color_data[1]);
        while(SPI_GetFlagStatus(SPI2,SPI_FLAG_TXE) == RESET);
    }
    while(SPI_GetFlagStatus(SPI2,SPI_FLAG_BSY) != RESET);//确保最后一件成功传达
    GPIO_SetBits(CS_PORT,CS_PIN);//拉高电平 

}

//8.送照片数据。把准备好的图片大数组，通过 SPI 运输车倒进地皮。
void st7789_draw_image(uint16_t x, uint16_t y, const image_t *image)
{
    if(x >= ST7789_WIDTH || y >= ST7789_HEIGHT || x + image->width -1 > ST7789_WIDTH || y + image->height +1 > ST7789_HEIGHT)
        return;

    st7789_set_range(x, y, x + image->width - 1, y + image->height - 1);
    st7789_set_gram_mode();

    GPIO_ResetBits(CS_PORT, CS_PIN);
    GPIO_SetBits(DC_PORT, DC_PIN);
    uint32_t size = image ->height *image->width * 2;
    const uint8_t *data = image ->data;
    for (uint32_t i = 0; i < size; i += 2 )
    {
        
        SPI_SendData(SPI2, data[i + 1]); while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
        SPI_SendData(SPI2, data[i]); while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
    
    }
    while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    GPIO_SetBits(CS_PORT, CS_PIN);
}


//9.送点阵数据（十字绣）。遇到图纸上的 '1' 送主题色，遇到 '0' 送背景色。
static void st7789_draw_font(uint16_t x, uint16_t y, uint16_t fwidth, uint16_t fheight, const uint8_t *model, uint16_t color, uint16_t bg_color)
{
    if(!in_screen_range(x, y, x + fwidth - 1, y + fheight - 1)) return;
    st7789_set_range(x, y, x + fwidth - 1, y + fheight - 1);
    st7789_set_gram_mode();

    uint8_t color_data[2] = {color >> 8, color & 0xFF};
    uint8_t bg_color_data[2] = {bg_color >> 8, bg_color & 0xFF};
    uint16_t bytes_per_row = (fwidth + 7)/8;//公式

    GPIO_ResetBits(CS_PORT, CS_PIN);
    GPIO_SetBits(DC_PORT, DC_PIN);

    for (uint16_t row = 0; row < fheight; row++)
    {
        const uint8_t *row_data = model + row * bytes_per_row;
        for(uint16_t col = 0; col < fwidth; col++)
        {
            uint8_t pixel = row_data[col/8] & (1 << (7 - col % 8));
            if(pixel)
            {
                SPI_SendData(SPI2, color_data[0]); while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
                SPI_SendData(SPI2, color_data[1]); while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
            }else{
                SPI_SendData(SPI2, bg_color_data[0]); while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);
                SPI_SendData(SPI2, bg_color_data[1]); while(SPI_GetFlagStatus(SPI2, SPI_FLAG_TXE) == RESET);

            }
        }
    }
    while(SPI_GetFlagStatus(SPI2, SPI_FLAG_BSY) != RESET);
    GPIO_SetBits(CS_PORT, CS_PIN);
}

//10.鉴定传来的数据是中文还是英文。
static bool is_gb2312(char ch)
{
    return ((unsigned char)ch >= 0xA1 && (unsigned char)ch <= 0xF7);
}

//11.把英文字母的数据提出来。
void st7789_write_ascii(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, const font_t *font)
{
    uint16_t fwidth  = font->size / 2; // 英文宽度是字体大小的一半 (如 16/2 = 8)
    uint16_t fheight = font->size;    // 高度就是字体大小 (如 16)
    char ch = *str;

    // 前天学过的公式！算出在英文大字典(ascii_model)里的起始位置
    uint16_t bytes_per_row = (fwidth + 7) / 8;
    const uint8_t *model = font->ascii_model + (ch - ' ') * fheight * bytes_per_row;
    //拿出当前字母 算算第几个 ×每个字的大小 加上起点位置 = model


    // 找到图纸了，扔给万能画笔！
    st7789_draw_font(x, y, fwidth, fheight, model, color, bg_color);
}
//12.把汉字的数据提出来。
static void st7789_write_chinese(uint16_t x, uint16_t y, char *ch, uint16_t color, uint16_t bg_color, const font_t *font)
{
    uint16_t fwidth  = font->size;  
    uint16_t fheight = font->size; 
    
    const font_chinese_t *c = font->chinese; //查找中文字典 
    for (; c->name != NULL; c++)
    {
        if (strcmp(c->name, ch) == 0) break;// 如果这页的名字(c->name)和要找的字(ch)一样，停！不翻了(break)！
    }
    
    if (c->name == NULL) return; 
    
    st7789_draw_font(x, y, fwidth, fheight, c->model, color, bg_color);
}
//13.终极组装流水线。把上面三个函数串起来，把一整句话的数据源源不断地送给画图女工。
void st7789_write_string(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg_color, const font_t *font)
{
    while (*str != '\0') 
    {
        if (is_gb2312(*str)) 
        {
            char ch[3] = {str[0], str[1], '\0'}; 
            st7789_write_chinese(x, y, ch, color, bg_color, font); 
            x = x + font->size; 
            str = str + 2;      
        } 
        else 
        {
            st7789_write_ascii(x, y, str, color, bg_color, font); 
            x = x + font->size / 2; 
            str = str + 1;          
        }
    }
}
//14.按下神圣的开关！控制 BL（Backlight）引脚输出高电平，点亮背光灯。瞬间，你输送到背板里的所有画面，跃然眼前！
static void st7789_set_backlight(bool on)
{
    GPIO_WriteBit(BL_PORT, BL_PIN, on ? Bit_SET : Bit_RESET);
    //BL 背光灯 writebit是BitSet和BitReset总和
    //on --> bool "?"表示如果为真选1 否则选二
}
