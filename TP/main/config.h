#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "portmacro.h"

#include "esp_lvgl_port.h"
#include "lvgl.h"


typedef enum {
  BTN_0 = GPIO_NUM_0,
  BTN_1 = GPIO_NUM_1,
  BTN_2 = GPIO_NUM_2
} gpio_input_t;

typedef enum {
  LED_VERDE = GPIO_NUM_4,
  LED_ROJO = GPIO_NUM_5,
  LED_DUMMY
} gpio_output_t;

void config_gpio(void);

lv_disp_t * getDisplay(void);

void config_lcd(void);

BaseType_t xConfig_test(void);

