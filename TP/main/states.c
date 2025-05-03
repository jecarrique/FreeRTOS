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
  /// --- Recursos para la tarea de cuenta del cronometro ---
  TaskHandle_t hndlTskContadorCronometro;
  TickType_t ultimo_evento; // = (TickType_t *)parametros;
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
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_CONTANDO");

        if (NULL == hndlTskContadorCronometro) {
          ESP_LOGI("Tarea Contador Cronometro", "Creandola...");

          ultimo_evento = xTaskGetTickCount();
          xTaskCreate(tskContadorCronometro, "CONTANDOR_CRONOMETRO",
                      configMINIMAL_STACK_SIZE * 4, &ultimo_evento,
                      tskIDLE_PRIORITY + 2, &hndlTskContadorCronometro);
          if (NULL == hndlTskContadorCronometro) {
            ESP_LOGI("Tarea Contador Cronometro", "Fallo la Creacion...");
          } else {
            ESP_LOGI("Tarea Contador Cronometro", "Ya esta Creada!");
          }
        }
      } else if (BTN_ACT == ulInterruptStatus) {
        states = RELOJ_IDLE;
        ESP_LOGI("Estado Nuevo", "RELOJ_IDLE");
      }
      break;

    case CRONOMETRO_CONTANDO:
      ESP_LOGI("Estado Actual", "CRONOMETRO_CONTANDO");
      /// Creo la tarea de cuenta

      if (NULL == hndlTskContadorCronometro) {
        ESP_LOGI("Tarea Contador Cronometro", "Creandola...");

        ultimo_evento = xTaskGetTickCount();
        xTaskCreate(tskContadorCronometro, "CONTANDOR_CRONOMETRO",
                    configMINIMAL_STACK_SIZE * 4, &ultimo_evento,
                    tskIDLE_PRIORITY + 2, &hndlTskContadorCronometro);
        if (NULL == hndlTskContadorCronometro) {
          ESP_LOGI("Tarea Contador Cronometro", "Fallo la Creacion...");
        } else {
          ESP_LOGI("Tarea Contador Cronometro", "Ya esta Creada!");
        }
      }

      // espero boton BTN_UP -> se detiene cuenta general y states = DETENIDO;
      // espero boton BTN_DOWN -> marca lap via xQueueSend

      if (BTN_UP == ulInterruptStatus) {
        states = CRONOMETRO_DETENIDO;
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_DETENIDO");

        if (NULL != hndlTskContadorCronometro) {
          ESP_LOGI("Tarea Contador Cronometro", "Borandola...");

          vTaskDelete(hndlTskContadorCronometro);
          if (NULL == hndlTskContadorCronometro) {
            ESP_LOGI("Tarea Contador Cronometro", "Borrado exitoso...");
          } else {
            ESP_LOGI("Tarea Contador Cronometro", "Fallo Borrado fallido...");
          }
        } else {
          ESP_LOGI("Tarea Contador Cronometro", "Ya esta Borrada!");
        }
      } else if (BTN_DOWN == ulInterruptStatus) {
        ESP_LOGI("NUEVO", "LAP");
      }

      break;

    case CRONOMETRO_DETENIDO:
      ESP_LOGI("Estado Actual", "CRONOMETRO_DETENIDO");
      /// Mato la tarea de cuenta
      // espero boton BTN_UP -> se reinicia la cuenta general desde donde estaba
      // y states = CONTANDO; espero boton BTN_ACT -> se borran los laps y la
      // cuenta se va a cero

      if (BTN_UP == ulInterruptStatus) {
        states = CRONOMETRO_CONTANDO;
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_CONTANDO");

        if (NULL == hndlTskContadorCronometro) {
            ESP_LOGI("Tarea Contador Cronometro", "Creandola...");
    
            ultimo_evento = xTaskGetTickCount();
            xTaskCreate(tskContadorCronometro, "CONTANDOR_CRONOMETRO",
                        configMINIMAL_STACK_SIZE * 4, &ultimo_evento,
                        tskIDLE_PRIORITY + 2, &hndlTskContadorCronometro);
            if (NULL == hndlTskContadorCronometro) {
              ESP_LOGI("Tarea Contador Cronometro", "Fallo la Creacion...");
            } else {
              ESP_LOGI("Tarea Contador Cronometro", "Ya esta Creada!");
            }
          }

      } else if (BTN_ACT == ulInterruptStatus) {
        states = CRONOMETRO_IDLE;
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_IDLE");
      }

      break;
    }
  }
}