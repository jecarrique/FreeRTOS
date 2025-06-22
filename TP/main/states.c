#include "comun.h"
#include "esp_log.h"

#include "clock.h"
#include "display.h"
#include "leds.h"
#include "states.h"

typedef enum {
  RELOJ_IDLE,
  RELOJ_ON,
  RELOJ_MENU_0,
  RELOJ_MENU_1,
  RELOJ_MENU_2,
  CHANGE_HORA,
  CHANGE_ALARM,
  CRONOMETRO_IDLE,
  CRONOMETRO_CONTANDO,
  CRONOMETRO_DETENIDO
} state_reloj_t;

void tskStates(void *parametros) {
  static state_reloj_t states = RELOJ_IDLE;
  uint32_t ulInterruptStatus;

  /// creo reloj
  init_time();

  /// --- creo timer para contar ---
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

  /// --- creo cola para laps y lanzo tare de laps ---
  static QueueHandle_t xQueueLaps;
  TaskHandle_t hndlTskLaps;
  xQueueLaps = xQueueCreate(3, sizeof(tiempo_comm_t));

  ESP_LOGE("Puntero a cola Laps", "%p", &xQueueLaps);
  ESP_LOGE("Valor como Puntero a cola Laps", "%p", xQueueLaps);
  if (NULL == xQueueLaps) {
    ESP_LOGE("ERROR", "Cola laps no creada");
  } else {
    ESP_LOGI("OK", "Cola laps creada");
  }
  UBaseType_t cnt = uxQueueMessagesWaiting(xQueueLaps);

  ESP_LOGE("", "la cola tiene %u", cnt);

  xTaskCreate(tskLaps, "LAPS", configMINIMAL_STACK_SIZE * 4,
              (void *)&xQueueLaps, tskIDLE_PRIORITY + 2, &hndlTskLaps);
  // tiempo_comm_t tiempo;
  // BaseType_t rta = xQueueSend(xQueueLaps, (void *) &tiempo, 100);
  /// lanzo ciclo infinito esperando se presionen las teclas para transicionar
  for (;;) {
    xTaskNotifyWaitIndexed(
        0,                  /* Wait for 0th Notificaition */
        0x00,               /* Don't clear any bits on entry. */
        ULONG_MAX,          /* Clear all bits on exit. */
        &ulInterruptStatus, /* Receives the notification value. */
        portMAX_DELAY);     /* Block indefinitely. */

    // ESP_LOGI("Boton recivido", "%lu\n", (unsigned long)ulInterruptStatus);

    switch (states) {
      ///-----------------------------------------------------------------
    case RELOJ_IDLE: // por ahora no hacenada.
      ESP_LOGI("Estado Actual", "RELOJ_IDLE");
      if (BTN_ACT == ulInterruptStatus) {
        states = RELOJ_ON; // CRONOMETRO_IDLE;
        ESP_LOGI("Estado Nuevo", "RELOJ_ON");
        delete_display();      // borro display general
        show_display_hora();   // muestro la hora
        update_display_hora(); // update_display_menu(0);
      }
      break;
      ///-----------------------------------------------------------------
    case RELOJ_ON: // por ahora no hacenada.
      ESP_LOGI("Estado Actual", "RELOJ_ON");
      if (BTN_ACT == ulInterruptStatus) {
        states = RELOJ_MENU_0; // CRONOMETRO_IDLE;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_0");
        hide_display_hora(); // no escribo la hora...
        delete_display();    // borro display general
        update_display_menu(0);
      }
      break;
    ///-----------------------------------------------------------------
    case RELOJ_MENU_0:
      ESP_LOGI("Estado Actual", "RELOJ_MENU_0");

      if (BTN_ACT == ulInterruptStatus) {
        states = CRONOMETRO_IDLE;
        ESP_LOGI("Estado Nuevo", "CRONOMETRO_IDLE");
        stopActionGreenLed();
        startActionRedLed();
        rst_crono();
        delete_display(); // borro display general
        reset_display_crono_laps();

        tiempo_comm_t tiempo;
        get_crono(&tiempo.partes);
        ESP_LOGW("cuenta actual", "%02d:%02d:%01d", tiempo.partes.mm,
                 tiempo.partes.ss, tiempo.partes.dd);
        delete_display(); // borro display general
        update_display_crono();
      }

      if (BTN_UP == ulInterruptStatus) {
        states = RELOJ_MENU_2;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_2");
        delete_display(); // borro display general
        update_display_menu(2);
      } else if (BTN_DOWN == ulInterruptStatus) {
        states = RELOJ_MENU_1;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_1");
        delete_display(); // borro display general
        update_display_menu(1);
      }
      break;

    ///-----------------------------------------------------------------
    case RELOJ_MENU_1:
      ESP_LOGI("Estado Actual", "RELOJ_MENU_1");

      if (BTN_UP == ulInterruptStatus) {
        states = RELOJ_MENU_0;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_0");
        delete_display(); // borro display general
        update_display_menu(0);
      } else if (BTN_DOWN == ulInterruptStatus) {
        states = RELOJ_MENU_2;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_2");
        delete_display(); // borro display general
        update_display_menu(2);
      }

      break;

    ///-----------------------------------------------------------------
    case RELOJ_MENU_2:
      ESP_LOGI("Estado Actual", "RELOJ_MENU_2");
      if (BTN_ACT == ulInterruptStatus) {
        states = CHANGE_HORA;
        ESP_LOGI("Estado Nuevo", "CHANGE_HORA");
        delete_display(); // borro display general
        bool test = update_display_chng_hora(ulInterruptStatus);

      }
      if (BTN_UP == ulInterruptStatus) {
        states = RELOJ_MENU_1;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_1");
        delete_display(); // borro display general
        update_display_menu(1);
      } else if (BTN_DOWN == ulInterruptStatus) {
        states = RELOJ_MENU_0;
        ESP_LOGI("Estado Nuevo", "RELOJ_MENU_0");
        delete_display(); // borro display general
        update_display_menu(0);
      }

      break;

    ///-----------------------------------------------------------------
    case CHANGE_HORA:
      ESP_LOGI("Estado Actual", "CHANGE_HORA");
      hide_display_hora();   // NO muestro la hora
      delete_display();      // borro display general
      bool test = update_display_chng_hora(ulInterruptStatus);
        if (true == test) {
          //if (BTN_ACT == ulInterruptStatus) {
            states = RELOJ_ON; // CRONOMETRO_IDLE;
            ESP_LOGI("Estado Nuevo", "RELOJ_ON");
            delete_display();      // borro display general
            show_display_hora();   // muestro la hora
            update_display_hora(); // update_display_menu(0);
          //}
        }
        break;

      ///-----------------------------------------------------------------
      case CHANGE_ALARM:
        break;

        ///-----------------------------------------------------------------
      case CRONOMETRO_IDLE:

        ESP_LOGI("Estado Actual", "CRONOMETRO_IDLE");
        rst_crono(); // Borro valr del contador
        reset_display_crono_laps();

        if (BTN_UP == ulInterruptStatus) {
          states = CRONOMETRO_CONTANDO;
          startActionGreenLed();
          stopActionRedLed();
          xTimerStart(xTimerContadorCronometro, 10); // se resetea el timer
          ESP_LOGI("Estado Nuevo", "CRONOMETRO_CONTANDO");
        }
        if (BTN_ACT == ulInterruptStatus) {
          states = RELOJ_ON; // CRONOMETRO_IDLE;
          ESP_LOGI("Estado Nuevo", "RELOJ_ON");
          delete_display();    // borro display general
          show_display_hora(); // muestro la hora
          update_display_hora();
        }
        break;
        ///-----------------------------------------------------------------
      case CRONOMETRO_CONTANDO:
        ESP_LOGI("Estado Actual", "CRONOMETRO_CONTANDO");
        /// Creo la tarea de cuenta

        // espero boton BTN_UP -> se detiene cuenta general y states = DETENIDO;
        // espero boton BTN_DOWN -> marca lap via xQueueSend

        if (BTN_UP == ulInterruptStatus) {
          states = CRONOMETRO_DETENIDO;
          xTimerStop(xTimerContadorCronometro, 10); // se detiene el timer
          stopActionGreenLed();
          startActionRedLed();

          ESP_LOGI("Estado Nuevo", "CRONOMETRO_DETENIDO");
        } else if (BTN_DOWN == ulInterruptStatus) {

          ESP_LOGI("Mantiene estado", "Manda Lap");
          tiempo_comm_t tiempo;
          get_crono(&tiempo.partes);
          ESP_LOGW("Lap", "%02d:%02d:%01d", tiempo.partes.mm, tiempo.partes.ss,
                   tiempo.partes.dd);
          update_display_crono();
          ESP_LOGW("Cola:", "intentando enviar dato");
          BaseType_t rta = xQueueSend(xQueueLaps, (void *)&tiempo, 100);
          if (errQUEUE_FULL == rta) {
            ESP_LOGW("Cola NoOk:", "llena, no se pudo enviar dato");
          } else {
            ESP_LOGW("Cola OK:", "se envio dato");
          }
        }
        break;
        ///-----------------------------------------------------------------
      case CRONOMETRO_DETENIDO:
        ESP_LOGI("Estado Actual", "CRONOMETRO_DETENIDO");
        /// Mato la tarea de cuenta
        // espero boton BTN_UP -> se reinicia la cuenta general desde donde
        // estaba y states = CONTANDO; espero boton BTN_ACT -> se borran los
        // laps y la cuenta se va a cero

        if (BTN_UP == ulInterruptStatus) {
          states = CRONOMETRO_CONTANDO;
          xTimerReset(xTimerContadorCronometro, 10); // se resetea el timer
          ESP_LOGI("Estado Nuevo", "CRONOMETRO_CONTANDO");
          startActionGreenLed();
          stopActionRedLed();

        } else if (BTN_ACT == ulInterruptStatus) {
          states = CRONOMETRO_IDLE;
          ESP_LOGI("Estado Nuevo", "CRONOMETRO_IDLE");
          rst_crono();
          reset_display_crono_laps();
          tiempo_comm_t tiempo;
          get_crono(&tiempo.partes);
          ESP_LOGW("cuenta actual", "%02d:%02d:%01d", tiempo.partes.mm,
                   tiempo.partes.ss, tiempo.partes.dd);
          update_display_crono();
          stopActionGreenLed();
          startActionRedLed();
        }
        break;
      }
    }
  }
