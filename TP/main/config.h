#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"

#include "esp_lvgl_port.h"
#include "lvgl.h"



void config_gpio(void);

lv_disp_t * getDisplay(void);

void config_lcd(void);

BaseType_t xConfig_test(void);

