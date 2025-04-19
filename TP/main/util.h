#ifndef UTIL_H
#define UTIL_H

cuenta_tiempo_t * getTiempo(void);


void tskBlink(void *parametros);
void tskKeys(void * parametres);
void tskContando(void *parametros);

void vActualizarDisplay(void);

#endif /* UTIL_H */