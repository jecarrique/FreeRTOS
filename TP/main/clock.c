#include "comun.h"

#include "clock.h"
#include "display.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

cuenta_tiempo_t clock = {0, 0, 0, 0};
#define ESPERA_MUTEX 1

// crea mutex y nicializa reloj en 0
BaseType_t init_time(void) {
  BaseType_t err = -1; // error
  clock.mutex = xSemaphoreCreateMutex();
  if (clock.mutex != NULL) {
    err = 0; // OK
  }
  return (err);
}

// incrementa las decimas del reloj
BaseType_t inc_time(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    if (clock.dd >= 9) {
      clock.dd = 0;
      if (clock.ss >= 59) {
        clock.ss = 0;
        if (clock.mm >= 59) {
          clock.mm = 0;
          if (clock.hh >= 23) {
            clock.hh = 0;
            /// overflow del contador...
          } else {
            ++clock.hh;
          }
        } else {
          ++clock.mm;
        }
      } else {
        ++clock.ss;
      }
    } else {
      ++clock.dd;
    }
    ptrTiempo->dd = clock.dd;
    ptrTiempo->hh = clock.hh;
    ptrTiempo->mm = clock.mm;
    ptrTiempo->ss = clock.ss;
    if (xSemaphoreGive(clock.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// pone en cero el reloj
BaseType_t rst_time(void) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock.dd = 0;
    clock.ss = 0;
    clock.mm = 0;
    clock.hh = 0;
    if (xSemaphoreGive(clock.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// devuelve el tiempo actual del reloj
BaseType_t get_time(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    ptrTiempo->dd = clock.dd;
    ptrTiempo->hh = clock.hh;
    ptrTiempo->mm = clock.mm;
    ptrTiempo->ss = clock.ss;

    if (xSemaphoreGive(clock.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// establece un valor determinado para el reloj
BaseType_t set_time(const tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock.dd = ptrTiempo->dd;
    clock.hh = ptrTiempo->hh;
    clock.mm = ptrTiempo->mm;
    clock.ss = ptrTiempo->ss;
    if (xSemaphoreGive(clock.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}

void vTimerCallback_ContadorCronometro(TimerHandle_t xTimer) {
  ESP_LOGW("Incremento contador cronometro", "Vamo arriba");
  tiempo_comm_t tiempo;
  inc_time(&tiempo.partes);
  ESP_LOGW("Cuenta", "%02d:%02d:%01d", tiempo.partes.mm, tiempo.partes.ss,
           tiempo.partes.dd);
  update_display_crono();
}

/// === tarea que genera la cuenta en el cronometro ===
void tskContadorCronometro(void *parametros) {
  TickType_t *ultimo_evento = (TickType_t *)parametros;
  // ultimo_evento = xTaskGetTickCount();
  for (;;) {
    tiempo_comm_t tiempo;
    inc_time(&tiempo.partes);
    BaseType_t xWasDelayed =
        xTaskDelayUntil((void *)ultimo_evento, pdMS_TO_TICKS(100));

    if (xWasDelayed == pdFALSE)
      ESP_LOGW("ATENCION", "FALLO vTaskDelayUntil");
  }
}

void tskLaps(void *parametros) {
  QueueHandle_t *colaLaps = (QueueHandle_t *)parametros;
  ESP_LOGE("Puntero a cola Laps dentro de la tskLaps", "%p", colaLaps);
  tiempo_comm_t laps_tiempo[] = {{0}, {0}, {0}}; /// inicializo laps en cero

  for (;;) {
    // vTaskDelay(100);
    /// espero lap y lo muestro en pantall
    tiempo_comm_t tiempo;
    //    UBaseType_t cnt = uxQueueMessagesWaiting(*colaLaps);
    //    if (  0 < cnt ) {
    //      ESP_LOGE("", "la cola tiene %u", cnt);

    BaseType_t xStatus;
    xStatus = xQueueReceive(*colaLaps, (void *)&tiempo,
                            portMAX_DELAY); // portMAX_DELAY);
    if (pdPASS == xStatus) {
      ESP_LOGE("Lap", "%02d:%02d:%01d", tiempo.partes.mm, tiempo.partes.ss,
               tiempo.partes.dd);
        
        laps_tiempo[2] = laps_tiempo[1]; 
        laps_tiempo[1] = laps_tiempo[0];
        laps_tiempo[0] = tiempo;
        for (int i = 0; i < 3; i++) {
          update_display_crono_label(i, laps_tiempo[i]);
        }
      

    } else {
      ESP_LOGE("", "la cola esta vacia!\r\n");
    }
  }
}