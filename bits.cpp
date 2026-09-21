#include "bits.h"

int bytesNecesarios(int filas, int columnas)
{
    int bitsTotales = 3 * filas * columnas;
    // Division entera "hacia arriba" (techo) sin usar <cmath>.
    return (bitsTotales + 7) / 8;
}

unsigned char extraerFicha(const unsigned char* tablero, int indice)
{
    int bitInicial = indice * 3;
    int byteInicial = bitInicial / 8;
    int offset = bitInicial % 8;

    // Se arma una ventana de 16 bits juntando el byte actual y el
    // siguiente. Esto es seguro incluso cuando la ficha esta
    // completamente contenida en byteInicial, porque en ese caso los
    // bits que se traen de byteInicial + 1 simplemente no se usan
    // (quedan fuera de la mascara final).
    unsigned short ventana = tablero[byteInicial];
    ventana = ventana | (static_cast<unsigned short>(tablero[byteInicial + 1]) << 8);

    unsigned char ficha = static_cast<unsigned char>((ventana >> offset) & MASCARA_3BITS);
    return ficha;
}

void insertarFicha(unsigned char* tablero, int indice, unsigned char valor)
{
    int bitInicial = indice * 3;
    int byteInicial = bitInicial / 8;
    int offset = bitInicial % 8;

    // Se asegura que 'valor' solo ocupe 3 bits, por seguridad.
    valor = valor & MASCARA_3BITS;

    if (offset <= 5) {
        // Caso 1: los 3 bits caben completos dentro de byteInicial.
        // Se borran esos 3 bits con una mascara invertida y luego se
        // colocan los nuevos con OR.
        unsigned char mascaraBorrado = ~(MASCARA_3BITS << offset);
        tablero[byteInicial] = (tablero[byteInicial] & mascaraBorrado) | (valor << offset);
    } else {
        // Caso 2: los 3 bits estan repartidos entre byteInicial y
        // byteInicial + 1 (offset es 6 o 7).
        // bitsEnPrimerByte indica cuantos bits de la ficha quedan en
        // el primer byte (2 si offset=6, 1 si offset=7).
        int bitsEnPrimerByte = 8 - offset;

        // --- Parte que va en el primer byte ---
        unsigned char mascaraBorrado1 = ~(MASCARA_3BITS << offset);
        unsigned char parteBaja = valor << offset;
        tablero[byteInicial] = (tablero[byteInicial] & mascaraBorrado1) | parteBaja;

        // --- Parte que va en el segundo byte ---
        // Se descartan los bits que ya se colocaron en el primer byte
        // y se ubican los restantes al inicio del segundo byte.
        unsigned char parteAlta = valor >> bitsEnPrimerByte;
        unsigned char bitsRestantes = 3 - bitsEnPrimerByte;
        unsigned char mascaraBorrado2 = ~(MASCARA_3BITS >> bitsEnPrimerByte);
        // mascaraBorrado2 borra unicamente los 'bitsRestantes' bits
        // menos significativos del segundo byte.
        tablero[byteInicial + 1] = (tablero[byteInicial + 1] & mascaraBorrado2) | parteAlta;

        // bitsRestantes se deja calculado para lectura del codigo,
        // aunque no se use explicitamente mas alla de este comentario:
        // sirve para verificar manualmente offset+bitsRestantes == 8.
        (void)bitsRestantes;
    }
}
