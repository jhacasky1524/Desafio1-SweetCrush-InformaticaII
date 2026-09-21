#include "rng.h"

// Estado interno del generador. xorshift no funciona con estado 0,
// por eso inicializarSemilla() nunca deja pasar ese valor.
static unsigned int estado = 2463534242u;

void inicializarSemilla(unsigned int semilla)
{
    estado = (semilla == 0) ? 1u : semilla;
}

// Un paso del algoritmo xorshift32: produce el siguiente numero de la
// secuencia pseudoaleatoria unicamente con XOR y desplazamientos.
static unsigned int siguienteAleatorio()
{
    estado = estado ^ (estado << 13);
    estado = estado ^ (estado >> 17);
    estado = estado ^ (estado << 5);
    return estado;
}

unsigned char fichaAleatoria()
{
    const unsigned int RANGO = 6;
    // Umbral de rechazo: descarta los valores que romperian la
    // uniformidad al aplicar el modulo (2^32 no es multiplo de 6).
    const unsigned int UMBRAL = 0xFFFFFFFFu - (0xFFFFFFFFu % RANGO);

    unsigned int valor;
    do {
        valor = siguienteAleatorio();
    } while (valor >= UMBRAL);

    return static_cast<unsigned char>(valor % RANGO);
}
