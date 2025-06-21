#include "comun.h"

#include "clock.h"
#include "display.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"

cuenta_tiempo_t clock_crono = {0, 0, 0, 0};
cuenta_tiempo_t clock_hora = {10, 15, 0, 0};
cuenta_tiempo_t clock_alarma = {10, 20, 0, 0};

//SemaphoreHandle_t mutex_hide_hora;//         mutex_hide_hora = xSemaphoreCreateMutex();

#define ESPERA_MUTEX 1

// crea mutex y nicializa reloj en 0
BaseType_t init_time(void) {
  BaseType_t err = -1; // error
  clock_crono.mutex = xSemaphoreCreateMutex();
  if (clock_crono.mutex != NULL) {
    err = 0; // OK
    clock_hora.mutex = xSemaphoreCreateMutex();
    if (clock_hora.mutex != NULL) {
      err = 0; // OK
      clock_alarma.mutex = xSemaphoreCreateMutex();
      if (clock_alarma.mutex != NULL) {
        err = 0; // OK
      } 
      else {
        err = -1; // error
      }
    }
    else {
      err = -1; // error
    }
  }
  return (err);
}
/// ---------------------------------------------------------------------------------
// devuelve el tiempo actual del reloj
BaseType_t get_alarma(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_alarma.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    ptrTiempo->dd = clock_alarma.dd;
    ptrTiempo->hh = clock_alarma.hh;
    ptrTiempo->mm = clock_alarma.mm;
    ptrTiempo->ss = clock_alarma.ss;

    if (xSemaphoreGive(clock_alarma.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}

// establece un valor determinado para el reloj
BaseType_t set_alarma(const tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_alarma.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock_alarma.dd = ptrTiempo->dd;
    clock_alarma.hh = ptrTiempo->hh;
    clock_alarma.mm = ptrTiempo->mm;
    clock_alarma.ss = ptrTiempo->ss;
    if (xSemaphoreGive(clock_alarma.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }
  return (err);
}
/// ---------------------------------------------------------------------------------

// incrementa minutos del reloj
BaseType_t inc_hora(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_hora.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
  //  if (clock_hora.dd >= 9) {
  //    clock_hora.dd = 0;
  //    if (clock_hora.ss >= 59) {
  //      clock_hora.ss = 0;
        if (clock_hora.mm >= 59) {
          clock_hora.mm = 0;
          if (clock_hora.hh >= 23) {
            clock_hora.hh = 0;
            /// overflow del contador...
          } else {
            ++clock_hora.hh;
          }
        } else {
          ++clock_hora.mm;
        }
  //    } else {
  //      ++clock_hora.ss;
  //    }
  //  } else {
  //    ++clock_hora.dd;
  //  }
    ptrTiempo->dd = clock_hora.dd;
    ptrTiempo->hh = clock_hora.hh;
    ptrTiempo->mm = clock_hora.mm;
    ptrTiempo->ss = clock_hora.ss;
    if (xSemaphoreGive(clock_hora.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// pone en cero el reloj
BaseType_t rst_hora(void) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_hora.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock_hora.dd = 0;
    clock_hora.ss = 0;
    clock_hora.mm = 0;
    clock_hora.hh = 0;
    if (xSemaphoreGive(clock_hora.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// devuelve el tiempo actual del reloj
BaseType_t get_hora(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_hora.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    ptrTiempo->dd = clock_hora.dd;
    ptrTiempo->hh = clock_hora.hh;
    ptrTiempo->mm = clock_hora.mm;
    ptrTiempo->ss = clock_hora.ss;

    if (xSemaphoreGive(clock_hora.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// establece un valor determinado para el reloj
BaseType_t set_hora(const tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_hora.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock_hora.dd = ptrTiempo->dd;
    clock_hora.hh = ptrTiempo->hh;
    clock_hora.mm = ptrTiempo->mm;
    clock_hora.ss = ptrTiempo->ss;
    if (xSemaphoreGive(clock_hora.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}

/// ---------------------------------------------------------------------------------
// incrementa las decimas del reloj
BaseType_t inc_crono(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_crono.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    if (clock_crono.dd >= 9) {
      clock_crono.dd = 0;
      if (clock_crono.ss >= 59) {
        clock_crono.ss = 0;
        if (clock_crono.mm >= 59) {
          clock_crono.mm = 0;
          if (clock_crono.hh >= 23) {
            clock_crono.hh = 0;
            /// overflow del contador...
          } else {
            ++clock_crono.hh;
          }
        } else {
          ++clock_crono.mm;
        }
      } else {
        ++clock_crono.ss;
      }
    } else {
      ++clock_crono.dd;
    }
    ptrTiempo->dd = clock_crono.dd;
    ptrTiempo->hh = clock_crono.hh;
    ptrTiempo->mm = clock_crono.mm;
    ptrTiempo->ss = clock_crono.ss;
    if (xSemaphoreGive(clock_crono.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// pone en cero el reloj
BaseType_t rst_crono(void) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_crono.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock_crono.dd = 0;
    clock_crono.ss = 0;
    clock_crono.mm = 0;
    clock_crono.hh = 0;
    if (xSemaphoreGive(clock_crono.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// devuelve el tiempo actual del reloj
BaseType_t get_crono(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_crono.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    ptrTiempo->dd = clock_crono.dd;
    ptrTiempo->hh = clock_crono.hh;
    ptrTiempo->mm = clock_crono.mm;
    ptrTiempo->ss = clock_crono.ss;

    if (xSemaphoreGive(clock_crono.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}
// establece un valor determinado para el reloj
BaseType_t set_crono(const tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(clock_crono.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    clock_crono.dd = ptrTiempo->dd;
    clock_crono.hh = ptrTiempo->hh;
    clock_crono.mm = ptrTiempo->mm;
    clock_crono.ss = ptrTiempo->ss;
    if (xSemaphoreGive(clock_crono.mutex) == pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}

void vTimerCallback_ContadorCronometro(TimerHandle_t xTimer) {
  ESP_LOGW("Incremento contador cronometro", "Vamo arriba");
  tiempo_comm_t tiempo;
  inc_crono(&tiempo.partes);
  ESP_LOGW("Cuenta", "%02d:%02d:%01d", tiempo.partes.mm, tiempo.partes.ss,
           tiempo.partes.dd);
  update_display_crono();
}

/// === tarea que genera la cuenta en el cronometro ===
void tskContadorHora(void *parametros) {
  //TickType_t *ultimo_evento = (TickType_t *)parametros;
  TickType_t ultimo_evento;
  ultimo_evento = xTaskGetTickCount();
  for (;;) {
    tiempo_comm_t tiempo;
    inc_hora(&tiempo.partes);
    ESP_LOGW("Hora:", "%02d:%02d", tiempo.partes.hh, tiempo.partes.mm);
    update_display_hora();
    BaseType_t xWasDelayed =
        xTaskDelayUntil(&ultimo_evento, pdMS_TO_TICKS(1000));

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
          update_display_crono_laps(i, laps_tiempo[i]);
        }
      

    } else {
      ESP_LOGE("", "la cola esta vacia!\r\n");
    }
  }
}