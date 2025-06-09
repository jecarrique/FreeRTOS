#include "comun.h"

#include "clock.h"
#include "config.h"
#include "util.h"
#include "keyboard.h"
#include "states.h"
#include "leds.h"
#include <stdio.h>


void app_main(void) {
  TaskHandle_t hndlTskToSuspend;

  init_time(); // 2do cheq err
  config_gpio();
  keyboard_init();
  init_inputs();
  config_lcd();
  init_leds();
  //xConfig_test();
  
  //gpio_set_level(LED_ROJO, 1);

  
  vTaskSuspendAll();  /// prevengo que las tareas tomen el CPU al ser creadas
  
  xTaskCreate(tskStates, "ESTADOS", configMINIMAL_STACK_SIZE * 4, NULL,
    tskIDLE_PRIORITY + 2, NULL);
  hndlTskToSuspend = xTaskGetHandle( "ESTADOS");
  vTaskSuspend( hndlTskToSuspend);

  xTaskCreate(tskContando, "CONTANDO", configMINIMAL_STACK_SIZE * 4, NULL,
    tskIDLE_PRIORITY + 2, NULL);
  hndlTskToSuspend = xTaskGetHandle( "CONTANDO");
  vTaskSuspend( hndlTskToSuspend);

  xTaskCreate(tskBlink, "BLINK", configMINIMAL_STACK_SIZE * 4, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  hndlTskToSuspend = xTaskGetHandle( "BLINK");
  vTaskSuspend( hndlTskToSuspend);

  //xTaskCreate(tskKeys, "KEYS", configMINIMAL_STACK_SIZE * 4, NULL,
  //  tskIDLE_PRIORITY + 1, NULL);
  //hndlTskToSuspend = xTaskGetHandle( "KEYS");
  //vTaskSuspend( hndlTskToSuspend);

  
  xTaskResumeAll(); /// ahora si largamos

  vTaskDelay(pdMS_TO_TICKS(1000));

  //hndlTskToSuspend = xTaskGetHandle( "BLINK");
  //vTaskResume( hndlTskToSuspend);

  //hndlTskToSuspend = xTaskGetHandle( "KEYS");
  //vTaskResume( hndlTskToSuspend);

  hndlTskToSuspend = xTaskGetHandle( "ESTADOS");
  vTaskResume( hndlTskToSuspend);


  for (;;) {

    vTaskDelay(pdMS_TO_TICKS(100));

  }
}
