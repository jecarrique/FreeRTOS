#include "comun.h"

#include "leds.h"

/// === privated stuff ===
void vTimerCallback_RedLedAction(TimerHandle_t xTimer) {
  // togle red led
  uint32_t pvTimerID_RedLedAction = ( uint32_t ) pvTimerGetTimerID( xTimer );

  if ( 0 == pvTimerID_RedLedAction ) { // si led apagado enciendo
    uint32_t pvTimerID_RedLedAction = 1; // arranca apagando
    gpio_set_level(LED_ROJO, pvTimerID_RedLedAction); // forzar estado led
    vTimerSetTimerID(xTimer, (void *)pvTimerID_RedLedAction);

  }
  else { // led encendido -> apago
    uint32_t pvTimerID_RedLedAction = 0; // arranca apagando
    gpio_set_level(LED_ROJO, pvTimerID_RedLedAction); // forzar estado led
    vTimerSetTimerID(xTimer, (void *)pvTimerID_RedLedAction);

  }
}

TimerHandle_t xTimerRedLedAction; /// creo puntero a timer

/// === public stuff ===
void init_leds(void) {
  /// --- creo timer para contar ---
  static uint32_t pvTimerID_RedLedAction = 0; // arranca apagando
  xTimerRedLedAction = xTimerCreate(
      /* Text name for the software timer - not used by FreeRTOS. */
      "timerContadorCronometro",
      /* The software timer's period in ticks. */
      pdMS_TO_TICKS(1000),
      /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
      pdTRUE,
      /* This example use the timer id. */
      (void *)&pvTimerID_RedLedAction,
      /* Callback function to be used by the software timer being created. */
      vTimerCallback_RedLedAction);
}

void startActionRedLed(void) {
  // void *pvTimerGetTimerID( xTimerRedLedAction ); // tomo ID para modificarlo
  if (xTimerReset(xTimerRedLedAction, pdMS_TO_TICKS(1000)) == pdPASS) {
    ESP_LOGI("led Rojo", "a titilar");
  } else {
    ESP_LOGE("led Rojo", "NO VA A TITILAR...");
  }
}
void stopActionRedLed(void) {

  if (xTimerStop(xTimerRedLedAction, pdMS_TO_TICKS(1000)) == pdPASS) {
    ESP_LOGI("led Rojo", "detenido");

    uint32_t pvTimerID_RedLedAction = 0; // arranca apagando
    gpio_set_level(LED_ROJO, pvTimerID_RedLedAction); // forzar estado led
    vTimerSetTimerID(xTimerRedLedAction, (void *)pvTimerID_RedLedAction);

  } else {
    ESP_LOGE("led Rojo", "NO VA A TITILAR...");
  }
}

void startActionGreenLed(void) {
    ESP_LOGI("Inicio Accion", "Led Verde");
    gpio_set_level(LED_VERDE, 1); // forzar estado led


}
void stopActionGreenLed(void) {
    ESP_LOGI("Detengo Accion", "Led Verde");
    gpio_set_level(LED_VERDE, 0); // forzar estado led

}
