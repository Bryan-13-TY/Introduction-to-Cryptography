#ifndef UTILS_H
#define UTILS_H

void wait_key();
void clean_buffer();

/**
 * @brief Lee una cadena de caracteres desde el teclado.
 * 
 * @param size Tamaño del arreglo de la cadena.
 * @param string Cadena leida.
 */
void read_string(int size, char string[size]);

#endif /* UTILS_H */