#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app.h"
#include "pages.h"

extern void board_lowlevel_init(void);
extern void board_init(void);

int main(void)
{
    board_lowlevel_init();
    board_init();

    welcome_page_display();

    esp32_app_init();
    wifi_page_display();
    esp32_wait_wifi_ready();

    main_loop_init();
    main_page_display();

    while (true)
    {
        main_loop();
    }
}
