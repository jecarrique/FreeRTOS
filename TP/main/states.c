#include "comun.h"
#include "esp_log.h"

#include "clock.h"
#include "states.h"

#define BTN_ACT BTN_0
#define BTN_DOWN BTN_1
#define BTN_UP BTN_2
typedef enum {
  RELOJ_IDLE,
  CRONOMETRO_IDLE,
  CRONOMETRO_CONTANDO,
  CRONOMETRO_DETENIDO
} state_reloj_t;

void tskStates(void *parametros) {
  static state_reloj_t states = RELOJ_IDLE;
  uint32_t ulInterruptStatus;

  /// creo timer para contar
  TimerHandle_t xTimerContadorCronometro; /// creo puntero a timer
  xTimerContadorCronometro = xTimerCreate(
    /* Text name for the software timer - not used by FreeRTOS. */
    "timerContadorCronometro",
    /* The software timer's period in ticks. */
    pdMS_TO_TICKS(100),
    /* Setting uxAutoRealod to pdFALSE creates a one-shot software timer. */
    pdTRUE,
    /* This example does not use the timer id. */
    0,
    /* Callback function to be used by the software timer being created. */
    vTimerCallback_ContadorCronometro);

    //xTimerStop( xTimerContadorCronometro, 0 );

  for (;;) {
    xTaskNotifyWaitIndexed(
        0,                  /* Wait for 0th Notificaition */
        0x00,               /* Don't clear any bits on entry. */
        ULONG_MAX,          /* Clear all bits on exit. */
        &ulInterruptStatus, /* Receives the notification value. */
        portMAX_DELAY);     /* Block indefinitely. */

    // ESP_LOGI("Boton recivido", "%lu\n", (unsigned long)ulInterruptStatus);

    switch (states) {
    case RELOJ_IDLE: // por ahora no hacenada.
      ESP_LOGI("Estado Actual", "RELOJ_IDLE");
      if (BTN_ACT == ulInterruptStatus) {
        states = CRONOMETRO_IDLE;
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_IDLE");
      }
    case CRONOMETRO_IDLE:
      
      ESP_LOGI("Estado Actual", "CRONOMETRO_IDLE");
      // espero boton BTN_UP -> inicio de cuenta,  states = CRONOMETRO_CONTANDO;
      // espero boton BTN_ACT -> Cambia a modo reloj (futuro)

      if (BTN_UP == ulInterruptStatus) {
        states = CRONOMETRO_CONTANDO;
        xTimerReset( xTimerContadorCronometro, 10 ); // se resetea el timer
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_CONTANDO");

        break;

      case CRONOMETRO_CONTANDO:
        ESP_LOGI("Estado Actual", "CRONOMETRO_CONTANDO");
        /// Creo la tarea de cuenta

        // espero boton BTN_UP -> se detiene cuenta general y states = DETENIDO;
        // espero boton BTN_DOWN -> marca lap via xQueueSend

        if (BTN_UP == ulInterruptStatus) {
          states = CRONOMETRO_DETENIDO;
          xTimerStop( xTimerContadorCronometro, 10 ); // se detiene el timer
          
          ESP_LOGI("Estado Nuevo", "CRONOMETRO_DETENIDO");

          break;

        case CRONOMETRO_DETENIDO:
          ESP_LOGI("Estado Actual", "CRONOMETRO_DETENIDO");
          /// Mato la tarea de cuenta
          // espero boton BTN_UP -> se reinicia la cuenta general desde donde
          // estaba y states = CONTANDO; espero boton BTN_ACT -> se borran los
          // laps y la cuenta se va a cero

          if (BTN_UP == ulInterruptStatus) {
            states = CRONOMETRO_CONTANDO;
            xTimerReset( xTimerContadorCronometro, 10 ); // se resetea el timer
            ESP_LOGI("Estado Nuevo", "CRONOMETRO_CONTANDO");

          } else if (BTN_ACT == ulInterruptStatus) {
            states = CRONOMETRO_IDLE;
            ESP_LOGI("Estado Nuevo", "CRONOMETRO_IDLE");
          }

          break;
        }
      }
    }
  }
}