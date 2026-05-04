#ifndef BLOCK_CRYPTO_H
#define BLOCK_CRYPTO_H

#include "block_status.h"

/**
 * @brief Genera una S-Box de 8 bits.
 * 
 * @return
 * - BLOCK_OK si la S-Box se genero correctamente.
 * - BLOCK_SBOX_GENERATION_ERROR si hubo un al generar y guardar la S-Box.
 * - BLOCK_SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
BLOCKStatus sbox_generator();

/**
 * @brief Genera una llave secreta `K` de 16 bits.
 * 
 * @return
 * - BLOCK_OK si la llave se genero correctamente.
 * - BLOCK_KEY_GENERATION_ERROR si hubo un error al generar y guardar la llave.
 */
BLOCKStatus secret_key_generator();

/**
 * @brief Genera una P-Box de tamaño 8.
 * 
 * @return
 * - BLOCK_OK si la P-Box se genero correctamente.
 * - BLOCK_PBOX_GENERATION_ERROR si hubo un error al generar y guardar la P-Box.
 */
BLOCKStatus pbox_generator();

/**
 * @brief Cifra un bloque de 2 bytes.
 * 
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param pbox_filename Archivo con la P-Box.
 * @param block Bloque a cifrar.
 * @param sub_keys Arreglo con las sub-llaves.
 * 
 * @return
 * - BLOCK_OK si el cifrado se hizo correctamente.
 * - BLOCK_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - BLOCK_KEY_READ_ERROR si hubo un error al leer la llave.
 * - BLOCK_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - BLOCK_PBOX_OPEN_FILE_ERROR si hubo un error al cargar la P-Box.
 * - BLOCK_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - BLOCK_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
BLOCKStatus encrypt(char sbox_filename[], char key_filename[], char pbox_filename[], char block[], unsigned short sub_keys[]);

/**
 * @brief Descifra un bloque de 2 bytes.
 * 
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param pbox_filename Archivo con la P-Box.
 * @param block Bloque a descifrar.
 * @param sub_keys Arreglo con las sub-llaves.
 * 
 * @return 
 * - BLOCK_OK si el descifrado se hizo correctamente. 
 * - BLOCK_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - BLOCK_KEY_READ_ERROR si hubo un error al leer la llave.
 * - BLOCK_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - BLOCK_PBOX_OPEN_FILE_ERROR si hubo un error al cargar la P-Box.
 * - BLOCK_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - BLOCK_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
BLOCKStatus decrypt(char sbox_filename[], char key_filename[], char pbox_filename[], unsigned short C, unsigned short sub_keys[]);

#endif /* BLOCK_CRYPTO_H */
