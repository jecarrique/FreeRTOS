#ifndef COMUN_H
#define COMUN_H
// === tipos comunes ===

#include "driver/gpio.h"

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

#endif /* COMUN_H */