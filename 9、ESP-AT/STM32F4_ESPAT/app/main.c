#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "cpu_tick.h"
#include "usart.h"
#include "esp_at.h"
#include "weather.h"

static const char *weather_url = "https://api.seniverse.com/v3/weather/now.json?key=SfRic8Wmp-Qh3OeFk&location=WTEMH46Z5N09&language=en&unit=c";

int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    cpu_tick_init();
    usart_init();
    printf("[SYS] startup\n");
    
    if (!esp_at_init())
    {
        printf("[AT] init failed\n");
        goto err;
    }
    printf("[AT] inited\n");
    
    if (!esp_at_wifi_init())
    {
        printf("[WIFI] init failed\n");
        goto err;
    }
    printf("[WIFI] inited\n");
    
    if (!esp_at_connect_wifi("Xiaomi Mi MIX 3_5577", "abc123456", NULL))
    {
        printf("[WIFI] connect failed\n");
        goto err;
    }
    printf("[WIFI] connecting\n");
    
    if (!esp_at_sntp_init())
    {
        printf("[SNTP] init failed\n");
        goto err;
    }
    printf("[SNTP] inited\n");
    
    while (1)
    {
        cpu_delay_ms(2000);
        
        esp_wifi_info_t wifi = { 0 };
        if (!esp_at_get_wifi_info(&wifi))
        {
            printf("[AT] wifi info get failed\n");
            continue;
        }
        
        if (!wifi.connected)
        {
            printf("[WIFI] disconnected\n");
            continue;
        }
        
        printf("[WIFI] SSID: %s, BSSID: %s, Channel: %d, RSSI: %d\n",
                wifi.ssid, wifi.bssid, wifi.channel, wifi.rssi);
        
        esp_date_time_t date = { 0 };
        if (!esp_at_sntp_get_time(&date))
        {
            printf("[SNTP] get time failed\n");
            continue;
        }
        
        if (date.year > 2000)
        {
            printf("[SNTP] %04u-%02u-%02u %02u:%02u:%02u (%s)\n",
                date.year, date.month, date.day, date.hour, date.minute, date.second,
                date.weekday == 1 ? "Monday":
                date.weekday == 2 ? "Tuesday":
                date.weekday == 3 ? "Wednesday":
                date.weekday == 4 ? "Thursday":
                date.weekday == 5 ? "Friday":
                date.weekday == 6 ? "Saturday":
                date.weekday == 7 ? "Sunday" : "Unknown");
        }
        
        weather_info_t weather = { 0 };
        const char *weather_http_response = esp_at_http_get(weather_url);
        if (weather_http_response == NULL)
        {
            printf("[WEAHTER] http error\n");
            continue;
        }
        
        if (!parse_seniverse_response(weather_http_response, &weather))
        {
            printf("[WEAHTER] parse failed\n");
            continue;
        }
        
        printf("[WEATHER] %s, %s, %.1f\n", weather.city, weather.weather, weather.temperature);
    }
    
err:
    while (1)
    {
        printf("AT Error\r\n");
        cpu_delay_ms(1000);
    }
}

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}
