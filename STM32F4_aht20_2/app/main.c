#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "aht20.h"
#include "usart.h"
#include "cpu_delay.h"
int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,  ENABLE);
    
    usart_init();
    if (!aht20_init())
    {
        printf("[ERROR] aht20 init failed!!!\r\n");
    }
    
    uint32_t failcount = 0;
    float temperature, humidity;
    while (1)
    {
        if (failcount >= 10)
        {
            printf("[ERROR] aht20 failed %u times! program stop!!!\r\n", failcount);
            break;
        }
        if (!aht20_start_measurement())
        {
            printf("[ERROR] aht20 start measurement failed!!!\r\n");
            failcount++;
            continue;
        }
        if (!aht20_wait_for_measurement())
        {
            printf("[ERROR] aht20 measurement timeout!!!\r\n");
            failcount++;
            continue;
        }
        if (!aht20_read_measurement(&temperature, &humidity))
        {
            printf("[ERROR] aht20 read measurement failed!!!\r\n");
            failcount++;
            continue;
        }
        
        failcount = 0;
        printf("temperature: %.2f, humidity:%.2f\r\n", temperature, humidity);
        cpu_delay(1000 * 1000);
    }
    
    while (1)
    {
        printf("system failed, please check log history\r\n");
        cpu_delay(1000 * 1000);
    }
}

int fputc(int ch, FILE *f)
{
    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_SendData(USART1, ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    return ch;
}

int main(void)
{
    aht20_init(); 
    
    uint8_t cmd[] = {0xAC, 0x33, 0x00};
    uint8_t data[6];

    while(1)
    {
        
        aht20_write(cmd, 3);
        for(int i=0; i<500000; i++); 
        aht20_read(data, 6);
        
        
    }
}
