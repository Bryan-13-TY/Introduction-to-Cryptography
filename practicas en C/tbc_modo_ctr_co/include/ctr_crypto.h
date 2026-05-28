#ifndef CTR_CRYPTO_H
#define CTR_CRYPTO_H

#include "ctr_status.h"

/**
 * @brief Genera una S-Box de 8 bits.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
CTRStatus sbox_generator();

/**
 * @brief Genera una llave secreta `K` de 16 bits.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
CTRStatus secret_key_generator();

/**
 * @brief Genera una P-Box de tamaño 8.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
CTRStatus pbox_generator();

/**
 * @brief Cifra un texto en bloques de 2 bytes usando el modo CTR.
 * 
 * @param plaintext_filename Archivo con el plaintext.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
CTRStatus encrypt_ctr(char plaintext_filename[]);

/**
 * @brief Descifra un texto en bloques de 2 bytes usando el modo CTR.
 * 
 * @param ciphertext_filename Archivo con el ciphertext.
 * 
 * @return CTRStatus Código de estado de la operación.
 */
CTRStatus decrypt_ctr(char ciphertext_filename[]);

#endif /* CTR_CRYPTO_H */
