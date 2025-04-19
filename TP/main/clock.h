#ifndef CLOCK_H
#define CLOCK_H

BaseType_t init_time( void ); // crea mutex y nicializa reloj en 0
BaseType_t inc_time( void ); // incrementa las decimas del reloj
BaseType_t rst_time( void ); // pone en cero el reloj
BaseType_t get_time( tiempo_t *ptrTiempo ); // devuelve el tiempo actual del reloj
BaseType_t set_time( const tiempo_t *ptrTiempo ); // establece un valor determinado para el reloj

#endif /* CLOCK_H */