#ifndef CTR_CRYPTO_H
#define CTR_CRYPTO_H

#include "ctr_status.h"

/**
 * @brief Genera una S-Box de 8 bits.
 * 
 * @return
 * - CTR_OK si la S-Box se genero correctamente.
 * - CTR_SBOX_GENERATION_ERROR si hubo un al generar y guardar la S-Box.
 * - CTR_SBOX_MEMORY_ERROR si hubo un error al reservar memoria para la S-Box.
 */
CTRStatus sbox_generator();

/**
 * @brief Genera una llave secreta `K` de 16 bits.
 * 
 * @return
 * - CTR_OK si la llave se genero correctamente.
 * - CTR_KEY_GENERATION_ERROR si hubo un error al generar y guardar la llave.
 */
CTRStatus secret_key_generator();

/**
 * @brief Genera una P-Box de tamaño 8.
 * 
 * @return
 * - CTR_OK si la P-Box se genero correctamente.
 * - CTR_PBOX_GENERATION_ERROR si hubo un error al generar y guardar la P-Box.
 */
CTRStatus pbox_generator();

/**
 * @brief Cifra un texto en bloques de 2 bytes.
 * 
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivo con la llave secreta.
 * @param pbox_filename Archivo con la P-Box.
 * @param plaintext Texto a cifrar.
 * @param size_plaintext Tamaño del `plaintext`.
 * 
 * @return
 * - CTR_OK si el cifrado se hizo correctamente.
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al abrir al archivo de la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo de la S-Box.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo de la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 * - CTR_CIPHER_GENERATION_ERROR si hubo un error al generar y guardar el texto cifrado.
 */
CTRStatus encrypt_ctr(
    char sbox_filename[],
    char key_filename[],
    char pbox_filename[],
    char plaintext[],
    int size_plaintext
);

/**
 * @brief Descifra un texto en bloques de 2 bytes.
 * 
 * @param sbox_filename Archivo con la S-Box.
 * @param key_filename Archivvo con la llave secreta.
 * @param pbox_filename Archivo con la P-Box.
 * @param ciphertext_filename Archivo con el texto cifrado.
 * 
 * @return
 * - CTR_OK si el descifrado se hizo correctamente.
 * - CTR_KEY_OPEN_FILE_ERROR si hubo un error al abrir al archivo de la llave.
 * - CTR_KEY_READ_ERROR si hubo un error al leer la llave.
 * - CTR_SBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo de la S-Box.
 * - CTR_PBOX_OPEN_FILE_ERROR si hubo un error al abrir el archivo de la P-Box.
 * - CTR_PBOX_OUT_OF_THE_RANGE_ERROR si el tamaño de la P-Box es incorrecto.
 * - CTR_PBOX_REPEATED_VALUES_ERROR si hay valores repetidos en la P-Box.
 * - CTR_CIPHER_OPEN_FILE_ERROR si hubo un error al abrir el archivo del ciphertext.
 * - CTR_COUNTER_READ_ERROR si hubo un error al leer el byte alto del contador del modo CTR.
 * - CTR_NUM_BLOCKS_READ_ERROR si hubo un error al leer el número de bloques del ciphertext.
 * - CTR_CIPHERTEXT_READ_ERROR si hubo un error al leer los bloques del ciphertext.
 */
CTRStatus decrypt_ctr(
    char sbox_filename[],
    char key_filename[],
    char pbox_filename[],
    char ciphertext_filename[]
);

#endif /* CTR_CRYPTO_H */
