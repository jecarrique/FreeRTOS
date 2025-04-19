#include "comun.h"

#include "clock.h"

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
BaseType_t inc_time(void) {
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
    if (xSemaphoreGive(clock.mutex) != pdTRUE) {
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
    if (xSemaphoreGive(clock.mutex) != pdTRUE) {
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

    if (xSemaphoreGive(clock.mutex) != pdTRUE) {
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
    if (xSemaphoreGive(clock.mutex) != pdTRUE) {
      err = 0; // OK
    }
  }

  return (err);
}