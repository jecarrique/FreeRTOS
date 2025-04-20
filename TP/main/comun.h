#ifndef COMUN_H
#define COMUN_H
// === tipos comunes ===

#include "freertos/FreeRTOS.h"
#include "esp_lvgl_port.h"
#include "driver/gpio.h"
#include "esp_log.h"


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

  typedef struct cuenta_tiempo_t {
    BaseType_t hh;
    BaseType_t mm;
    BaseType_t ss;
    BaseType_t dd;
    SemaphoreHandle_t mutex;
  } cuenta_tiempo_t;

  typedef struct tiempo_t {
    BaseType_t hh;
    BaseType_t mm;
    BaseType_t ss;
    BaseType_t dd;
  } tiempo_t;  
#endif /* COMUN_H */