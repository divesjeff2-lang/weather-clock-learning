#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "rtc.h"
#include "cpu_tick.h"
#include "aht20.h"
#include "esp_at.h"
#include "weather.h"
#include "pages.h"
#include "app.h"

#define MILLISECONDS(x) (x)
#define SECONDS(x) ((x) * 1000)
#define MINUTES(x) (SECONDS(x) * 60)
#define HOURS(x) (MINUTES(x) * 60)
#define DAYS(x) (HOURS(x) * 24)

#define TIME_SYNC_INTERVAL      DAYS(1)
#define WIFI_UPDATE_INTERVAL    SECONDS(5)
#define TIME_UPDATE_INTERVAL    MILLISECONDS(50)
#define OUTER_UPDATE_INTERVAL   MINUTES(10)
#define INNER_UPDATE_INTERVAL   SECONDS(10)

static uint32_t time_sync_delay; // 1d更新
static uint32_t wifi_update_delay; // 5s更新
static uint32_t time_update_delay; // 1s更新
static uint32_t inner_update_delay; // 10s更新
static uint32_t outer_update_delay; // 10m更新

static void cpu_period_callback(void)
{
    if (time_sync_delay > 0)
        time_sync_delay--;
    if (wifi_update_delay > 0)
        wifi_update_delay--;
    if (time_update_delay > 0)
        time_update_delay--;
    if (outer_update_delay > 0)
        outer_update_delay--;
    if (inner_update_delay > 0)
        inner_update_delay--;
}

void main_loop_init(void)
{
    cpu_register_periodic_callback(cpu_period_callback);
}

static void time_sync(void)
{
    if (time_sync_delay)
        return;

    time_sync_delay = TIME_SYNC_INTERVAL;

    esp_date_time_t esp_date = { 0 };
    if (!esp_at_sntp_get_time(&esp_date))
    {
        printf("[SNTP] get time failed\n");
        time_sync_delay = SECONDS(5);
        return;
    }

    if (esp_date.year <= 2000)
    {
        printf("[SNTP] invalid date received\n");
        time_sync_delay = SECONDS(1);
        return;
    }

    printf("[SNTP] sync time: %04u-%02u-%02u %02u:%02u:%02u (%d)\n",
        esp_date.year, esp_date.month, esp_date.day,
        esp_date.hour, esp_date.minute, esp_date.second, esp_date.weekday);

    rtc_date_time_t rtc_date = { 0 };
    rtc_date.year = esp_date.year;
    rtc_date.month = esp_date.month;
    rtc_date.day = esp_date.day;
    rtc_date.hour = esp_date.hour;
    rtc_date.minute = esp_date.minute;
    rtc_date.second = esp_date.second;
    rtc_date.weekday = esp_date.weekday;
    rtc_set_time(&rtc_date);

    time_update_delay = 0;
}

static void wifi_update(void)
{
    static esp_wifi_info_t last_wifi_info = { 0 };

    if (wifi_update_delay)
        return;

    wifi_update_delay = WIFI_UPDATE_INTERVAL;

    esp_wifi_info_t wifi = { 0 };
    if (!esp_at_get_wifi_info(&wifi))
    {
        printf("[AT] wifi info get failed\n");
        return;
    }

    if (memcmp(&wifi, &last_wifi_info, sizeof(esp_wifi_info_t)) == 0)
    {
        return;
    }

    if (last_wifi_info.connected == wifi.connected)
    {
        return;
    }

    if (wifi.connected)
    {
        printf("[WIFI] connected to %s\n", wifi.ssid);
        printf("[WIFI] SSID: %s, BSSID: %s, Channel: %d, RSSI: %d\n",
                wifi.ssid, wifi.bssid, wifi.channel, wifi.rssi);
        main_page_redraw_wifi_ssid(wifi.ssid);
    }
    else
    {
        printf("[WIFI] disconnected from %s\n", last_wifi_info.ssid);
        main_page_redraw_wifi_ssid("wifi lost");
    }

    memcpy(&last_wifi_info, &wifi, sizeof(esp_wifi_info_t));
}

static void time_update(void)
{
    static rtc_date_time_t last_date = { 0 };

    if (time_update_delay)
        return;

    time_update_delay = TIME_UPDATE_INTERVAL;

    rtc_date_time_t date;
    rtc_get_time(&date);

    if (memcmp(&date, &last_date, sizeof(rtc_date_time_t)) == 0)
    {
        return;
    }

    memcpy(&last_date, &date, sizeof(rtc_date_time_t));
    main_page_redraw_time(&date);
    main_page_redraw_date(&date);
}

static void inner_update(void)
{
    static float last_inner_temperature = 0.0f;
    static float last_inner_humidity = 0.0f;

    if (inner_update_delay)
        return;

    inner_update_delay = INNER_UPDATE_INTERVAL;

    if (!aht20_start_measurement())
    {
        printf("[AHT20] start measurement failed\n");
        return;
    }

    if (!aht20_wait_for_measurement())
    {
        printf("[AHT20] wait for measurement failed\n");
        return;
    }

    float temperature = 0.0f;
    float humidity = 0.0f;
    aht20_wait_for_measurement();
    if (!aht20_read_measurement(&temperature, &humidity))
    {
        printf("[AHT20] get data failed\n");
        return;
    }

    if (temperature == last_inner_temperature && humidity == last_inner_humidity)
    {
        return;
    }

    last_inner_temperature = temperature;
    last_inner_humidity = humidity;

    printf("[AHT20] Temperature: %.1f, Humidity: %.1f\n", temperature, humidity);
    main_page_redraw_inner_temperature(temperature);
    main_page_redraw_inner_humidity(humidity);
}

static void outer_update(void)
{
    static weather_info_t last_weather = { 0 };

    if (outer_update_delay)
        return;

    outer_update_delay = OUTER_UPDATE_INTERVAL;

    weather_info_t weather = { 0 };
    const char *weather_http_response = esp_at_http_get(weather_url);
    if (weather_http_response == NULL)
    {
        printf("[WEAHTER] http error\n");
        outer_update_delay = SECONDS(5);
        return;
    }

    if (!parse_seniverse_response(weather_http_response, &weather))
    {
        printf("[WEAHTER] parse failed\n");
        outer_update_delay = SECONDS(5);
        return;
    }

    if (memcmp(&weather, &last_weather, sizeof(weather_info_t)) == 0)
    {
        return;
    }

    memcpy(&last_weather, &weather, sizeof(weather_info_t));
    printf("[WEATHER] %s, %s, %.1f\n", weather.city, weather.weather, weather.temperature);
    main_page_redraw_outdoor_temperature(weather.temperature);
    main_page_redraw_outdoor_weather_icon(weather.weather_code);
}

void main_loop(void)
{
    time_sync();
    wifi_update();
    time_update();
    inner_update();
    outer_update();
}
