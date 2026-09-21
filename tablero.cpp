#include "tablero.h"
#include "bits.h"
#include "rng.h"
#include <iostream>

// Umbral de la regla de liberacion de memoria (65%), expresado como
// fraccion para evitar trabajar con numeros de punto flotante donde
// no es indispensable: memoriaNecesaria * 100 < 65 * memoriaReservada.
static const int UMBRAL_NUMERADOR = 65;
static const int UMBRAL_DENOMINADOR = 100;

// ---------------------------------------------------------------
// Traduccion (fila, columna) <-> indice logico
// ---------------------------------------------------------------
unsigned char obtenerFicha(const unsigned char* tablero, int columnas, int fila, int columna)
{
    int indice = fila * columnas + columna;
    return extraerFicha(tablero, indice);
}

void colocarFicha(unsigned char* tablero, int columnas, int fila, int columna, unsigned char valor)
{
    int indice = fila * columnas + columna;
    insertarFicha(tablero, indice, valor);
}

// ---------------------------------------------------------------
// Simbolo en pantalla para cada codigo de ficha
// ---------------------------------------------------------------
static char simboloDeFicha(unsigned char valor)
{
    switch (valor) {
        case FICHA_A: return 'A';
        case FICHA_B: return 'B';
        case FICHA_C: return 'C';
        case FICHA_D: return 'D';
        case FICHA_E: return 'E';
        case FICHA_F: return 'F';
        case ESTADO_ESPECIAL: return '*';
        case POSICION_VACIA: return '.';
        default: return '?';
    }
}

// Imprime los 3 bits de 'valor' como texto ("000".."111"), leyendo
// bit a bit de mas significativo a menos significativo.
static void imprimirBinario(unsigned char valor)
{
    for (int b = 2; b >= 0; b--) {
        std::cout << (((valor >> b) & 1) ? '1' : '0');
    }
}

// ---------------------------------------------------------------
// Creacion del tablero sin combinaciones iniciales
// ---------------------------------------------------------------

// Verifica si colocar 'valor' en (fila, columna) formaria una
// combinacion horizontal con las DOS fichas anteriores de la misma
// fila (ya colocadas, porque se llena en orden fila por fila).
static bool formariaComboHorizontal(const unsigned char* tablero, int columnas, int fila, int columna, unsigned char valor)
{
    if (columna < 2) return false;
    unsigned char v1 = obtenerFicha(tablero, columnas, fila, columna - 1);
    unsigned char v2 = obtenerFicha(tablero, columnas, fila, columna - 2);
    return (v1 == valor && v2 == valor);
}

// Igual que la anterior, pero verificando las dos fichas de arriba en
// la misma columna (ya colocadas, porque se llena fila por fila).
static bool formariaComboVertical(const unsigned char* tablero, int columnas, int fila, int columna, unsigned char valor)
{
    if (fila < 2) return false;
    unsigned char v1 = obtenerFicha(tablero, columnas, fila - 1, columna);
    unsigned char v2 = obtenerFicha(tablero, columnas, fila - 2, columna);
    return (v1 == valor && v2 == valor);
}

static void llenarAleatorioSinCombinaciones(unsigned char* tablero, int filas, int columnas)
{
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            unsigned char valor;
            do {
                valor = fichaAleatoria();
            } while (formariaComboHorizontal(tablero, columnas, fila, columna, valor)
                     || formariaComboVertical(tablero, columnas, fila, columna, valor));
            colocarFicha(tablero, columnas, fila, columna, valor);
        }
    }
}

void crearTablero(unsigned char*& tablero, int filas, int columnas, int& bytesReservados)
{
    bytesReservados = bytesNecesarios(filas, columnas);
    tablero = new unsigned char[bytesReservados];
    for (int i = 0; i < bytesReservados; i++) tablero[i] = 0;
    llenarAleatorioSinCombinaciones(tablero, filas, columnas);
}

void liberarTablero(unsigned char*& tablero)
{
    delete[] tablero;
    tablero = nullptr;
}

// ---------------------------------------------------------------
// Visualizacion
// ---------------------------------------------------------------
void mostrarTablero(const unsigned char* tablero, int filas, int columnas, bool modoBinario)
{
    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            unsigned char valor = obtenerFicha(tablero, columnas, fila, columna);
            if (modoBinario) {
                imprimirBinario(valor);
                std::cout << ' ';
            } else {
                std::cout << simboloDeFicha(valor) << ' ';
            }
        }
        std::cout << std::endl;
    }
}

// ---------------------------------------------------------------
// Insertar / eliminar filas
// ---------------------------------------------------------------
void agregarFila(unsigned char*& tablero, int& filas, int columnas, int& bytesReservados, int filaDestino)
{
    int filasNuevas = filas + 1;
    int bytesNuevos = bytesNecesarios(filasNuevas, columnas);

    unsigned char* nuevoTablero = new unsigned char[bytesNuevos];
    for (int i = 0; i < bytesNuevos; i++) nuevoTablero[i] = 0;

    for (int fila = 0; fila < filasNuevas; fila++) {
        for (int columna = 0; columna < columnas; columna++) {
            unsigned char valor;
            if (fila == filaDestino) {
                valor = fichaAleatoria();
            } else {
                int filaOrigen = (fila < filaDestino) ? fila : fila - 1;
                valor = obtenerFicha(tablero, columnas, filaOrigen, columna);
            }
            colocarFicha(nuevoTablero, columnas, fila, columna, valor);
        }
    }

    delete[] tablero;
    tablero = nuevoTablero;
    filas = filasNuevas;
    bytesReservados = bytesNuevos;
}

void eliminarFila(unsigned char*& tablero, int& filas, int columnas, int& bytesReservados, int filaAEliminar)
{
    int filasNuevas = filas - 1;
    int bytesNecesariosNuevos = bytesNecesarios(filasNuevas, columnas);

    // Se compacta primero en un buffer temporal del tamanio EXACTO que
    // se necesitaria, para poder comparar contra la regla del 65% sin
    // tocar todavia la memoria fisica actual.
    unsigned char* temporal = new unsigned char[bytesNecesariosNuevos];
    for (int i = 0; i < bytesNecesariosNuevos; i++) temporal[i] = 0;

    for (int fila = 0; fila < filasNuevas; fila++) {
        int filaOrigen = (fila < filaAEliminar) ? fila : fila + 1;
        for (int columna = 0; columna < columnas; columna++) {
            unsigned char valor = obtenerFicha(tablero, columnas, filaOrigen, columna);
            colocarFicha(temporal, columnas, fila, columna, valor);
        }
    }

    // Regla del 65%: bytesNecesariosNuevos < 0.65 * bytesReservados
    bool debeReducirMemoria = (bytesNecesariosNuevos * UMBRAL_DENOMINADOR) < (UMBRAL_NUMERADOR * bytesReservados);

    if (debeReducirMemoria) {
        delete[] tablero;
        tablero = temporal;
        bytesReservados = bytesNecesariosNuevos;
    } else {
        // Se conserva la capacidad ya reservada: se copia el
        // contenido compactado de vuelta al buffer actual (que sigue
        // siendo suficientemente grande) y se descarta el temporal.
        for (int i = 0; i < filasNuevas * columnas; i++) {
            insertarFicha(tablero, i, extraerFicha(temporal, i));
        }
        delete[] temporal;
    }

    filas = filasNuevas;
}

// ---------------------------------------------------------------
// Insertar / eliminar columnas
// ---------------------------------------------------------------
void agregarColumna(unsigned char*& tablero, int filas, int& columnas, int& bytesReservados, int columnaDestino)
{
    int columnasNuevas = columnas + 1;
    int bytesNuevos = bytesNecesarios(filas, columnasNuevas);

    unsigned char* nuevoTablero = new unsigned char[bytesNuevos];
    for (int i = 0; i < bytesNuevos; i++) nuevoTablero[i] = 0;

    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnasNuevas; columna++) {
            unsigned char valor;
            if (columna == columnaDestino) {
                valor = fichaAleatoria();
            } else {
                int columnaOrigen = (columna < columnaDestino) ? columna : columna - 1;
                valor = obtenerFicha(tablero, columnas, fila, columnaOrigen);
            }
            colocarFicha(nuevoTablero, columnasNuevas, fila, columna, valor);
        }
    }

    delete[] tablero;
    tablero = nuevoTablero;
    columnas = columnasNuevas;
    bytesReservados = bytesNuevos;
}

void eliminarColumna(unsigned char*& tablero, int filas, int& columnas, int& bytesReservados, int columnaAEliminar)
{
    int columnasNuevas = columnas - 1;
    int bytesNecesariosNuevos = bytesNecesarios(filas, columnasNuevas);

    unsigned char* temporal = new unsigned char[bytesNecesariosNuevos];
    for (int i = 0; i < bytesNecesariosNuevos; i++) temporal[i] = 0;

    for (int fila = 0; fila < filas; fila++) {
        for (int columna = 0; columna < columnasNuevas; columna++) {
            int columnaOrigen = (columna < columnaAEliminar) ? columna : columna + 1;
            unsigned char valor = obtenerFicha(tablero, columnas, fila, columnaOrigen);
            colocarFicha(temporal, columnasNuevas, fila, columna, valor);
        }
    }

    bool debeReducirMemoria = (bytesNecesariosNuevos * UMBRAL_DENOMINADOR) < (UMBRAL_NUMERADOR * bytesReservados);

    if (debeReducirMemoria) {
        delete[] tablero;
        tablero = temporal;
        bytesReservados = bytesNecesariosNuevos;
    } else {
        for (int i = 0; i < filas * columnasNuevas; i++) {
            insertarFicha(tablero, i, extraerFicha(temporal, i));
        }
        delete[] temporal;
    }

    columnas = columnasNuevas;
}
