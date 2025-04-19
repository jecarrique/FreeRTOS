#include "comun.h"

#include "clock.h"

cuenta_tiempo_t tiempo = {0, 0, 0, 0};
#define ESPERA_MUTEX 1

// crea mutex y nicializa reloj en 0
BaseType_t init_time(void) {
  BaseType_t err = -1; // error
  tiempo.mutex = xSemaphoreCreateMutex();
  if (tiempo.mutex != NULL) {
    err = 0; // OK
  }
  return (err);
}
// incrementa las decimas del reloj
BaseType_t inc_time(void) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(tiempo.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    if (tiempo.dd >= 9) {
      tiempo.dd = 0;
      if (tiempo.ss >= 59) {
        tiempo.ss = 0;
        if (tiempo.mm >= 59) {
          tiempo.mm = 0;
          if (tiempo.hh >= 23) {
            tiempo.hh = 0;
            /// overflow del contador...
          } else {
            ++tiempo.hh;
          }
        } else {
          ++tiempo.mm;
        }
      } else {
        ++tiempo.ss;
      }
    } else {
      ++tiempo.dd;
    }
    err = 0; // OK
  }

  return (err);
}
// pone en cero el reloj
BaseType_t rst_time(void) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(tiempo.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    tiempo.dd = 0;
    tiempo.ss = 0;
    tiempo.mm = 0;
    tiempo.hh = 0;
    err = 0; // OK
  }

  return (err);
}
// devuelve el tiempo actual del reloj
BaseType_t get_time(tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(tiempo.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    ptrTiempo->dd = tiempo.dd;
    ptrTiempo->hh = tiempo.hh;
    ptrTiempo->mm = tiempo.mm;
    ptrTiempo->ss = tiempo.ss;

    err = 0; // OK
  }

  return (err);
}
// establece un valor determinado para el reloj
BaseType_t set_time(const tiempo_t *ptrTiempo) {
  BaseType_t err = -1; // error
  if (xSemaphoreTake(tiempo.mutex, pdMS_TO_TICKS(ESPERA_MUTEX)) == pdTRUE) {
    tiempo.dd = ptrTiempo->dd;
    tiempo.hh = ptrTiempo->hh;
    tiempo.mm = ptrTiempo->mm;
    tiempo.ss = ptrTiempo->ss;
    err = 0; // OK
  }

  return (err);
}