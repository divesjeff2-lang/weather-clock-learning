#include <stdint.h>
#include <stddef.h>
#include "stm32f4xx.h"
#include "cpu_delay.h"
#include "st7789.h"

//SPI_CLK ---PB13
//SPI_MOSI ---PC3
//SPI_MISO ---PC2
//SPI_CS ---PE2
//SPI_reset ---PE3
//SPI_DC ---PE4
//SPI_LED ---PE5

#define CS_PORT     GPIOE
#define CS_PIN      GPIO_Pin_2
#define RESET_PORT  GPIOE
#define RESET_PIN   GPIO_Pin_3
#define DC_PORT     GPIOE
#define DC_PIN      GPIO_Pin_4
#define BL_PORT     GPIOE
#define BL_PIN      GPIO_Pin_5

#define delay_ms(x) cpu_delay((x)*1000)
#define delay_us(x) cpu_delay((x)

static void st7789_init_display(void);

void st7789_init(void)
{
    // Initialize GPIO and SPI for ST7789
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    
    GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5); // Set CS, RESET, DC, BL pins high
    GPIO_ResetBits(BL_PORT, BL_PIN);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_SPI2);

    SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_Cmd(SPI2, ENABLE);
    
    st7789_init_display();
}

static void st7789_write_register(uint8_t reg, uint8_t data[], uint16_t length)
{
    GPIO_ResetBits(CS_PORT, CS_PIN); // Command mode

    GPIO_ResetBits(DC_PORT, DC_PIN); // Command mode
    SPI_SendData(SPI2, reg);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); // Wait until transmit buffer is empty
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET); // Wait until transmit buffer is empty

    GPIO_SetBits(DC_PORT, DC_PIN); // Data mode
    for(uint16_t i = 0; i < length; i++)
    {
        SPI_SendData(SPI2, data[i]);
        while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)); // Wait until transmit buffer is empty
    }
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET); // Wait until transmit buffer is empty

    GPIO_SetBits(CS_PORT, CS_PIN); // Deselect chip
}

static void st7789_reset(void)
{
    GPIO_ResetBits(RESET_PORT, RESET_PIN);
    delay_ms(20);
    GPIO_SetBits(RESET_PORT, RESET_PIN);
    delay_ms(120);
}

static void st7789_set_backlight(bool on)
{
    GPIO_WriteBit(BL_PORT, BL_PIN, on ? Bit_SET : Bit_RESET);
}


static void st7789_init_display(void)
{
    st7789_reset();

    st7789_write_register(0x11, NULL, 0); // Sleep Out
    delay_ms(5);

    st7789_write_register(0x36, (uint8_t[]){0x00}, 1); // Memory Data Access Control
    st7789_write_register(0x3A, (uint8_t[]){0x05}, 1); // Interface Pixel Format
    st7789_write_register(0xB7, (uint8_t[]){0x46}, 1); // Gate Control 
    st7789_write_register(0xBB, (uint8_t[]){0x1B}, 1); // VCOM Setting
    st7789_write_register(0xC0, (uint8_t[]){0x2C}, 1); // LCM Control
    st7789_write_register(0xC2, (uint8_t[]){0x01}, 1); // VDV and VRH Command Enable
    st7789_write_register(0xC4, (uint8_t[]){0x20}, 1); // VDV Setting
    st7789_write_register(0xC6, (uint8_t[]){0x0F}, 1); // Frame Rate Control in Normal Mode
    st7789_write_register(0xD0, (uint8_t[]){0xA4, 0xA1}, 2); // Power Control 1
    st7789_write_register(0xD6, (uint8_t[]){0xA1}, 1); // Positive Voltage Gamma Control
    st7789_write_register(0xE0, (uint8_t[]){0xF0, 0x00, 0x06, 0x04, 0x05, 0x05, 0x31, 0x44, 0x48, 0x36, 0x12, 0x12, 0x2B, 0x34}, 14); // Positive Voltage Gamma Control
    st7789_write_register(0xE0, (uint8_t[]){0xF0, 0x0B, 0x0F, 0x0F, 0x0D, 0x26, 0x31, 0x43, 0x47, 0x38, 0x14, 0x14, 0x2C, 0x32}, 14); // Negative Voltage Gamma Control
    st7789_write_register(0x21, NULL, 0); // Display Inversion On
    st7789_write_register(0x29, NULL, 0); // Display On

    st7789_fill_color(0, 0, ST7789_WIDTH, ST7789_HEIGHT, 0xFFFF); // Fill screen with white color
    st7789_set_backlight(true);
}

static bool in_screen_ranger(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    return x + width <= ST7789_WIDTH &&
           y + height <= ST7789_HEIGHT &&
           x < ST7789_WIDTH &&
           y < ST7789_HEIGHT;
}

static void st7789_set_range(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    st7789_write_register(0x2A, (uint8_t[]){(x >> 8) & 0xFF, x & 0xFF, (width >> 8) & 0xFF, width & 0xFF}, 4); // Column Address Set
    st7789_write_register(0x2B, (uint8_t[]){(y >> 8) & 0xFF, y & 0xFF, (height >> 8) & 0xFF, height & 0xFF}, 4); // Row Address Set
}

static void st7789_set_gram_mode(void)
{
    st7789_write_register(0x2C, NULL, 0); // Memory Write
}

void st7789_fill_color(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    if (!in_screen_ranger(x, y, width, height))
        return;

    st7789_set_range(x, y, width, height);
    st7789_set_gram_mode();

    uint8_t color_data[2] = {(color >> 8) & 0xFF, color & 0xFF};

    GPIO_ResetBits(CS_PORT, CS_PIN); // Command mode
    GPIO_SetBits(DC_PORT, DC_PIN); // Data mode
    SPI_SendData(SPI2, 0x2C);
    for(uint16_t i = 0; i < width * height; i++)
    {
        SPI_SendData(SPI2, color_data[0]);
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); // Wait until transmit buffer is empty
        SPI_SendData(SPI2, color_data[1]);
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); // Wait until transmit buffer is empty
    }
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET); // Wait until transmit buffer is empty

    GPIO_SetBits(CS_PORT, CS_PIN); // Deselect chip
}

