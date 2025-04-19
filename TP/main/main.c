#include "comun.h"
#include "clock.h"
#include "config.h"
#include "util.h"


#include <stdio.h>


void app_main(void) {
  TaskHandle_t hndlTskToSuspend;

  init_time(); // 2do cheq err
  config_gpio();
  config_lcd();
  //xConfig_test();
  
  gpio_set_level(LED_ROJO, 1);

  
  vTaskSuspendAll();  /// prevengo que las tareas tomen el CPU al ser creadas
   
  xTaskCreate(tskBlink, "BLINK", configMINIMAL_STACK_SIZE * 4, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  hndlTskToSuspend = xTaskGetHandle( "BLINK");
  vTaskSuspend( hndlTskToSuspend);

  xTaskCreate(tskKeys, "KEYS", configMINIMAL_STACK_SIZE * 4, NULL,
    tskIDLE_PRIORITY + 1, NULL);
  hndlTskToSuspend = xTaskGetHandle( "KEYS");
  vTaskSuspend( hndlTskToSuspend);

  xTaskCreate(tskContando, "CONTANDO", configMINIMAL_STACK_SIZE * 4, NULL,
    tskIDLE_PRIORITY + 5, NULL);
  hndlTskToSuspend = xTaskGetHandle( "CONTANDO");
  vTaskSuspend( hndlTskToSuspend);

  xTaskResumeAll(); /// ahora si largamos

  vTaskDelay(pdMS_TO_TICKS(2000));

  //hndlTskToSuspend = xTaskGetHandle( "BLINK");
  //vTaskResume( hndlTskToSuspend);

  hndlTskToSuspend = xTaskGetHandle( "KEYS");
  vTaskResume( hndlTskToSuspend);

  for (;;) {

    vTaskDelay(pdMS_TO_TICKS(100));

  }
}
