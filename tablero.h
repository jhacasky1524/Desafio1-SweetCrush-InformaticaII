#ifndef TABLERO_H
#define TABLERO_H

/*
 * Modulo: tablero
 * ---------------
 * Responsable de la creacion, visualizacion y modificacion estructural
 * (agregar/eliminar filas y columnas) del tablero de juego. Traduce
 * siempre coordenadas (fila, columna) a un indice logico y delega en
 * el modulo "bits" toda lectura/escritura real de los 3 bits de cada
 * posicion. Ninguna funcion de este modulo manipula bytes directamente.
 *
 * El tablero se describe con tres variables independientes (no se usa
 * struct/class, prohibidos por el enunciado):
 *   unsigned char* tablero      -> bloque de memoria dinamica compacto
 *   int filas, int columnas     -> dimensiones logicas actuales
 *   int bytesReservados         -> tamanio fisico realmente reservado
 *
 * bytesReservados puede ser MAYOR que bytesNecesarios(filas,columnas):
 * eso ocurre despues de eliminar filas/columnas mientras la ocupacion
 * no haya caido por debajo del 65%, tal como exige el enunciado.
 */

// Crea el tablero: reserva la memoria minima necesaria y lo llena con
// fichas aleatorias sin que se formen combinaciones de 3 o mas ya
// resueltas desde el inicio.
void crearTablero(unsigned char*& tablero, int filas, int columnas, int& bytesReservados);

// Libera la memoria del tablero y deja el puntero en nullptr.
void liberarTablero(unsigned char*& tablero);

// Lee/escribe una ficha usando coordenadas (fila, columna) en lugar
// del indice logico crudo (evita que otros modulos calculen a mano
// fila * columnas + columna).
unsigned char obtenerFicha(const unsigned char* tablero, int columnas, int fila, int columna);
void colocarFicha(unsigned char* tablero, int columnas, int fila, int columna, unsigned char valor);

// Muestra el tablero completo. Si modoBinario es true, cada posicion
// se imprime como sus 3 bits (por ejemplo "101"); si es false, se
// imprime el simbolo asociado a la ficha (A..F, '*' o '.').
void mostrarTablero(const unsigned char* tablero, int filas, int columnas, bool modoBinario);

// Inserta una fila nueva (rellena con fichas aleatorias) justo en la
// posicion 'filaDestino' (0..filas), desplazando las filas siguientes.
// Siempre reserva memoria nueva del tamanio exacto requerido.
void agregarFila(unsigned char*& tablero, int& filas, int columnas, int& bytesReservados, int filaDestino);

// Elimina la fila 'filaAEliminar' (0..filas-1). Aplica la regla del
// 65%: solo redimensiona la memoria fisica si la ocupacion resultante
// cae por debajo del 65% de lo actualmente reservado.
void eliminarFila(unsigned char*& tablero, int& filas, int columnas, int& bytesReservados, int filaAEliminar);

// Inserta una columna nueva (rellena con fichas aleatorias) en la
// posicion 'columnaDestino' (0..columnas), desplazando las columnas
// siguientes en cada fila. Siempre reserva memoria nueva exacta.
void agregarColumna(unsigned char*& tablero, int filas, int& columnas, int& bytesReservados, int columnaDestino);

// Elimina la columna 'columnaAEliminar' (0..columnas-1). Aplica la
// misma regla del 65% que eliminarFila.
void eliminarColumna(unsigned char*& tablero, int filas, int& columnas, int& bytesReservados, int columnaAEliminar);

#endif // TABLERO_H
