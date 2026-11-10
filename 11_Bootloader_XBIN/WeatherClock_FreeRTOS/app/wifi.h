#ifndef __WIFI_H__
#define __WIFI_H__

#define APP_VERSION "v1.0"
#define WIFI_SSID   "your_wifi_ssid"
#define WIFI_PASSWD "your_wifi_password"

void wifi_init(void);
void wifi_wait_connect(void);


#endif /* __WIFI_H__ */
