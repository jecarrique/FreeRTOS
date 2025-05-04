#include "comun.h"

#include "leds.h"

/// === privated stuff ===
TimerHandle_t xTimerRedLedAction; /// creo puntero a timer
TimerHandle_t xTimerGreenLedAction; /// creo puntero a timer

void vTimerCallback_RedLedAction(TimerHandle_t xTimer) {
  // togle red led
  uint32_t pvTimerID_LedAction = ( uint32_t ) pvTimerGetTimerID( xTimer );

  if ( 0 == pvTimerID_LedAction ) { // si led apagado enciendo
    uint32_t pvTimerID_LedAction = 1; // arranca apagando
    gpio_set_level(LED_ROJO, pvTimerID_LedAction); // forzar estado led
    vTimerSetTimerID(xTimer, (void *)pvTimerID_LedAction);

  }
  else { // led encendido -> apago
    uint32_t pvTimerID_LedAction = 0; // arranca apagando
    gpio_set_level(LED_ROJO, pvTimerID_LedAction); // forzar estado led
    vTimerSetTimerID(xTimer, (void *)pvTimerID_LedAction);

  }
}

void vTimerCallback_GreenLedAction(TimerHandle_t xTimer) {
    // togle red led
    uint32_t pvTimerID_LedAction = ( uint32_t ) pvTimerGetTimerID( xTimer );
  
    if ( 0 == pvTimerID_LedAction ) { // si led apagado enciendo
    ESP_LOGI("Estado LED Verde", "ENCENDIDO");
      uint32_t pvTimerID_LedAction = 1; // arranca apagando
      gpio_set_level(LED_VERDE, pvTimerID_LedAction); // forzar estado led
      vTimerSetTimerID(xTimer, (void *)pvTimerID_LedAction);
  
    }
    else { // led encendido -> apago
        ESP_LOGI("Estado LED Verde", "APAGADO");
      uint32_t pvTimerID_LedAction = 0; // arranca apagando
      gpio_set_level(LED_VERDE, pvTimerID_LedAction); // forzar estado led
      vTimerSetTimerID(xTimer, (void *)pvTimerID_LedAction);
  
    }
  }
  

/// === public stuff ===
void init_leds(void) {
  /// --- creo timer para contar ---
  static uint32_t pvTimerID_RedLedAction = 1; // arranca encendido
  static uint32_t pvTimerID_GreenLedAction = 1; // arranca encendido

  xTimerRedLedAction = xTimerCreate(
      /* Text name for the software timer - not used by FreeRTOS. */
      "xTimerRedLedAction",
      /* The software timer's period in ticks. */
      pdMS_TO_TICKS(1000),
      /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
      pdTRUE,
      /* This example use the timer id. */
      (void *)&pvTimerID_RedLedAction,
      /* Callback function to be used by the software timer being created. */
      vTimerCallback_RedLedAction);

      xTimerGreenLedAction = xTimerCreate(
        /* Text name for the software timer - not used by FreeRTOS. */
        "xTimerGreenLedAction",
        /* The software timer's period in ticks. */
        pdMS_TO_TICKS(1000),
        /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
        pdTRUE,
        /* This example use the timer id. */
        (void *)&pvTimerID_GreenLedAction,
        /* Callback function to be used by the software timer being created. */
        vTimerCallback_GreenLedAction);
     
}

void startActionGreenLed(void) {
  // void *pvTimerGetTimerID( xTimerRedLedAction ); // tomo ID para modificarlo
  if (xTimerReset(xTimerGreenLedAction, pdMS_TO_TICKS(1000)) == pdPASS) {
    ESP_LOGI("led Verde", "a titilar");
    ESP_LOGI("Estado LED Verde", "ENCENDIDO INICIALMENTE");
    uint32_t pvTimerID_LedAction = 1; // arranca encendido
    gpio_set_level(LED_VERDE, pvTimerID_LedAction); // forzar estado led
    vTimerSetTimerID(xTimerGreenLedAction, (void *)pvTimerID_LedAction);

  } else {
    ESP_LOGE("led Verde", "NO VA A TITILAR...");
  }
}
void stopActionGreenLed(void) {

  if (xTimerStop(xTimerGreenLedAction, pdMS_TO_TICKS(1000)) == pdPASS) {
    ESP_LOGI("led Verde", "detenido");

    uint32_t pvTimerID_LedAction = 0; // arranca apagando
    gpio_set_level(LED_VERDE, pvTimerID_LedAction); // forzar estado led
    vTimerSetTimerID(xTimerGreenLedAction, (void *)pvTimerID_LedAction);

  } else {
    ESP_LOGE("led Verde", "NO VA A TITILAR...");
  }
}

void startActionRedLed(void) {
    ESP_LOGI("Inicio Accion", "Led Rojo");
    gpio_set_level(LED_ROJO, 1); // forzar estado led
}

void stopActionRedLed(void) {
    ESP_LOGI("Detengo Accion", "Led Rojo");
    gpio_set_level(LED_ROJO, 0); // forzar estado led

}
