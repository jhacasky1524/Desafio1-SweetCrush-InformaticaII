#ifndef RNG_H
#define RNG_H

/*
 * Modulo: rng
 * -----------
 * Generador de numeros pseudoaleatorios propio (xorshift32), construido
 * unicamente con operadores a nivel de bits (^, <<, >>). Se evita
 * deliberadamente rand()/srand() de <cstdlib> para no depender de
 * funciones de estilo C y para reforzar el uso de bitwise en el core
 * de la solucion.
 */

// Inicializa el estado interno del generador. Debe llamarse una sola
// vez al inicio del programa, con una semilla que cambie en cada
// ejecucion (por ejemplo, la hora actual en milisegundos).
void inicializarSemilla(unsigned int semilla);

// Devuelve un codigo de ficha valido (0..5) siguiendo una distribucion
// uniforme, usando "rejection sampling" para no introducir sesgo
// (6 no divide exactamente a 2^32).
unsigned char fichaAleatoria();

#endif // RNG_H
