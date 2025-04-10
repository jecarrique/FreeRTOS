#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"

typedef struct cuenta_tiempo_t {
    BaseType_t hh;
    BaseType_t mm;
    BaseType_t ss;
    BaseType_t dd;
    SemaphoreHandle_t mutex;
  } cuenta_tiempo_t;

cuenta_tiempo_t * getTiempo(void);


void tskBlink(void *parametros);
void tskKeys(void * parametres);
void tskContando(void *parametros);

void vActualizarDisplay(void);
