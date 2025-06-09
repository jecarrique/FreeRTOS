#ifndef CLOCK_H
#define CLOCK_H

BaseType_t init_time( void ); // crea mutex y inicializa reloj en 0 pa






BaseType_t inc_hora( tiempo_t *ptrHora ); // incrementa los minutos del reloj

BaseType_t rst_hora( void ); // pone en cero el reloj
BaseType_t get_hora( tiempo_t *ptrHora ); // devuelve el tiempo actual del reloj
BaseType_t set_hora( const tiempo_t *ptrHora ); // establece un valor determinado para el reloj


BaseType_t inc_crono( tiempo_t *ptrCrono ); // incrementa las decimas del reloj
BaseType_t rst_crono( void ); // pone en cero el reloj
BaseType_t get_crono( tiempo_t *ptrCrono ); // devuelve el tiempo actual del reloj
BaseType_t set_crono( const tiempo_t *ptrCrono ); // establece un valor determinado para el reloj
void vTimerCallback_ContadorCronometro(TimerHandle_t xTimer); // cuenta del cronometro...

void tskContadorHora(void *parametros); // tarea para manejar la cuenta, me gusta la idea de que fuera estatica.


void tskLaps(void *parametros);

#endif /* CLOCK_H */