#ifndef BITS_H
#define BITS_H

/*
 * Modulo: bits
 * ------------
 * Unico modulo que manipula directamente la memoria fisica del tablero
 * (el arreglo de unsigned char). Ninguna otra parte del programa debe
 * usar operadores de bits sobre el arreglo del tablero: todo acceso o
 * modificacion de una ficha pasa obligatoriamente por estas funciones.
 *
 * Cada posicion logica del tablero ocupa EXACTAMENTE 3 bits dentro de
 * una secuencia continua de bits (sin relleno para alinear a byte).
 */

// ---- Codigos de 3 bits para cada estado posible de una posicion ----
// Los seis primeros representan las seis fichas del juego.
const unsigned char FICHA_A = 0b000;
const unsigned char FICHA_B = 0b001;
const unsigned char FICHA_C = 0b010;
const unsigned char FICHA_D = 0b011;
const unsigned char FICHA_E = 0b100;
const unsigned char FICHA_F = 0b101;
// Codigo reservado para un estado especial (uso futuro / bonus).
const unsigned char ESTADO_ESPECIAL = 0b110;
// Codigo que representa una posicion vacia (tras una eliminacion).
const unsigned char POSICION_VACIA = 0b111;

// Mascara de 3 bits, usada para aislar el valor de una ficha.
const unsigned char MASCARA_3BITS = 0b111;

/*
 * bytesNecesarios
 * ---------------
 * Calcula la cantidad minima de bytes requerida para almacenar un
 * tablero de 'filas' x 'columnas' posiciones, sabiendo que cada
 * posicion ocupa exactamente 3 bits.
 *
 * Formula: techo(3 * filas * columnas / 8), calculada con aritmetica
 * entera para no depender de <cmath>.
 */
int bytesNecesarios(int filas, int columnas);

/*
 * extraerFicha
 * ------------
 * Devuelve el codigo de 3 bits (0..7) almacenado en la posicion
 * logica 'indice' (indice = fila * columnas + columna) dentro de la
 * secuencia compacta de bits apuntada por 'tablero'.
 *
 * Funciona sin importar si los 3 bits de la ficha estan contenidos en
 * un solo byte o repartidos entre dos bytes consecutivos: se arma una
 * "ventana" de 16 bits (byte actual + byte siguiente) y se extrae el
 * valor con un desplazamiento y una mascara, sin necesidad de un
 * caso especial (if) para cada situacion.
 */
unsigned char extraerFicha(const unsigned char* tablero, int indice);

/*
 * insertarFicha
 * -------------
 * Escribe el codigo de 3 bits 'valor' (0..7) en la posicion logica
 * 'indice' de la secuencia compacta de bits, sin alterar ningun otro
 * bit del tablero. A diferencia de la lectura, la escritura si debe
 * distinguir si la ficha cabe en un solo byte o esta repartida entre
 * dos, porque hay que modificar uno o dos bytes fisicos segun el caso.
 */
void insertarFicha(unsigned char* tablero, int indice, unsigned char valor);

#endif // BITS_H
