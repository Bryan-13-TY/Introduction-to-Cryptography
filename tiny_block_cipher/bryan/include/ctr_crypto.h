#ifndef CTR_CRYPTO_H
#define CTR_CRYPTO_H

#include "ctr_status.h"

/**
 * @brief Genera una S-Box de 8 bits.
 * 
 * @return
 * - CTR_OK si la S-Box se genero correctamente.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al guardar la S-Box.
 * - CTR_SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
CTRStatus sbox_generator();

/**
 * @brief Genera una llave secreta `K` de 16 bits.
 * 
 * @return
 * - CTR_OK si la llave se genero correctamente.
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al guardar la llave.
 */
CTRStatus secret_key_generator();

/**
 * @brief Genera una P-Box de tamaño 8.
 * 
 * @return
 * - CTR_OK si la P-Box se genero correctamente.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al guardar la P-Box.
 */
CTRStatus pbox_generator();

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
 * - CTR_OK si el cifrado se hizo correctamente.
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al cargar la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
CTRStatus encrypt(char sbox_filename[], char key_filename[], char pbox_filename[], char block[], unsigned short sub_keys[]);

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
 * - CTR_OK si el descifrado se hizo correctamente. 
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al cargar la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al cargar la S-Box.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al cargar la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 */
CTRStatus decrypt(char sbox_filename[], char key_filename[], char pbox_filename[], unsigned short C, unsigned short sub_keys[]);

#endif /* CTR_H */