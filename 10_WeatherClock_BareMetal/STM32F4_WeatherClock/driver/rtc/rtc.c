#include <stdint.h>
#include <string.h>
#include "stm32f4xx.h"
#include "rtc.h"

void rtc_init(void)
{
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_ON);

    uint32_t timeout = 0x10000;
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && timeout--);

    if (timeout == 0)
    {
        RCC_LSICmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    }
    else
    {
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    }

    RCC_RTCCLKCmd(ENABLE);
    while (RTC_WaitForSynchro() == ERROR);

    RTC_InitTypeDef RTC_InitStruct;
    RTC_StructInit(&RTC_InitStruct);
    RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStruct);
}

void rtc_set_time(const rtc_date_time_t *date_time)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    time.RTC_Hours = date_time->hour;
    time.RTC_Minutes = date_time->minute;
    time.RTC_Seconds = date_time->second;
    time.RTC_H12 = RTC_H12_AM; // 24小时格式下此字段无意义，设为默认值
    RTC_SetTime(RTC_Format_BIN, &time);

    date.RTC_WeekDay = date_time->weekday;
    date.RTC_Month = date_time->month;
    date.RTC_Date = date_time->day;
    date.RTC_Year = date_time->year % 100; // Year in BCD format
    RTC_SetDate(RTC_Format_BIN, &date);
}

static void _rtc_get_time_once(rtc_date_time_t *date_time)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    RTC_GetTime(RTC_Format_BIN, &time);
    RTC_GetDate(RTC_Format_BIN, &date);

    date_time->hour = time.RTC_Hours;
    date_time->minute = time.RTC_Minutes;
    date_time->second = time.RTC_Seconds;
    date_time->weekday = date.RTC_WeekDay;
    date_time->month = date.RTC_Month;
    date_time->day = date.RTC_Date;
    date_time->year = 2000 + date.RTC_Year;
}

void rtc_get_time(rtc_date_time_t *date_time)
{
    rtc_date_time_t time1, time2;

    do {
        _rtc_get_time_once(&time1);
        _rtc_get_time_once(&time2);
    } while (memcmp(&time1, &time2, sizeof(rtc_date_time_t)) != 0);

    memcpy(date_time, &time2, sizeof(rtc_date_time_t));
}
