#ifndef COMUN_H
#define COMUN_H
// === tipos comunes ===

#include "freertos/FreeRTOS.h"
#include "esp_lvgl_port.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "soc/gpio_reg.h" // lectura directa a registro de gpio
#include <stdint.h>       // tipos de datos estandar


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
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
    uint8_t dd;
  } tiempo_t;  

  typedef union tiempo_comm_t {
    uint32_t todo;
    tiempo_t partes;
  } tiempo_comm_t;
#endif /* COMUN_H */