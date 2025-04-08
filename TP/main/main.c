#include "config.h"

#include <stdio.h>

void tskBlink(void *parametros);

void app_main(void) {
  TaskHandle_t hndlTskToSuspend;
  config_gpio();
  config_lcd();
  xConfig_test();
  
  vTaskSuspendAll();  /// prevengo que las tareas tomen el CPU al ser creadas
   
  xTaskCreate(tskBlink, "ROJO", configMINIMAL_STACK_SIZE * 4, NULL,
              tskIDLE_PRIORITY + 1, NULL);
  hndlTskToSuspend = xTaskGetHandle( "ROJO");
  vTaskSuspend( hndlTskToSuspend);

  xTaskResumeAll(); /// ahora si largamos

  vTaskDelay(pdMS_TO_TICKS(2000));

  hndlTskToSuspend = xTaskGetHandle( "ROJO");
  vTaskResume( hndlTskToSuspend);

  for (;;) {

    vTaskDelay(pdMS_TO_TICKS(10));

  }
}
void tskBlink(void *parametros) {
  TickType_t ultimo_evento;
  ultimo_evento = xTaskGetTickCount();

  for(;;){
    gpio_set_level(GPIO_NUM_5,1);
    vTaskDelayUntil(&ultimo_evento, pdMS_TO_TICKS(500));
    gpio_set_level(GPIO_NUM_5,0);
    vTaskDelayUntil(&ultimo_evento, pdMS_TO_TICKS(500));

  }
}