#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "bl24c512.h"
#include "usart.h"

#define EEPROM_SN_OFFSET    120
#define EEPROM_BOOT_COUNT   30

int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    
    usart_init();
    bl24c512_init();
    
    const char default_sn[] = "mhdev_1234567890";
    char sn[17];
    if (bl24c512_read(EEPROM_SN_OFFSET, (uint8_t *)sn, 17))
    {
        if (memcmp(default_sn, sn, 6) == 0)
        {
            printf("board sn: %s\r\n", sn);
        }
        else
        {
            printf("[WARN] eeprom re-write default sn: %s\r\n", default_sn);
            if (!bl24c512_write(EEPROM_SN_OFFSET, (uint8_t*)default_sn, sizeof(default_sn)))
            {
                printf("[ERROR] eeprom write default sn failed!!!\r\n");
            }
        }
    }
    else
    {
        printf("[ERROR] eeprom raed sn failed!!!\r\n");
    }
    
    union { uint32_t value; uint8_t data[4]; } boot_count;
    if (bl24c512_read(EEPROM_BOOT_COUNT, boot_count.data, sizeof(boot_count)))
    {
        if (boot_count.value < 100ul * 10000ul)
        {
            boot_count.value++;
        }
        else
        {
            boot_count.value = 0;
        }
        printf("boot count: %u\r\n", boot_count.value);
        if (!bl24c512_write(EEPROM_BOOT_COUNT, boot_count.data, sizeof(boot_count)))
        {
            printf("[ERROR] eeprom write boot count failed!!!\r\n");
        }
    }
    else
    {
        printf("[ERROR] eeprom raed boot count failed!!!\r\n");
    }
    
    
    while(1)
    {
        ;
    }
}

int fputc(int ch, FILE *f)
{
    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_SendData(USART1, ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return ch;
}
