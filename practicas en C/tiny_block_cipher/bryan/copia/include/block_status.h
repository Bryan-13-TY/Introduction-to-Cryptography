#ifndef BLOCK_STATUS_H
#define BLOCK_STATUS_H

/**
 * @enum BLOCKStatus
 * @brief Códigos para el block cipher.
 * 
 * Las funciones para generar una S-Box, P-Box, llave secreta,
 * cifrar y descifrar devuelven uno de estos valores para
 * indicar éxito o el tipo de error ocurrido.
 * 
 */
typedef enum {
    BLOCK_OK = 0, /**< Operación exitosa */
    BLOCK_SBOX_OPEN_FILE_ERROR = -1, /**< Error al abrir el archivo de la S-Box */
    BLOCK_SBOX_GENERATION_ERROR = -2, /**< Error al generar la S-Box */
    BLOCK_SBOX_MEMORY_ERROR = -3, /**< Error al reservar memoria para la S-Box */
    BLOCK_KEY_OPEN_FILE_ERROR = -4, /**< Error al abrir el archivo de la llave */
    BLOCK_KEY_GENERATION_ERROR = -5, /**< Error al generar la llave */
    BLOCK_KEY_READ_ERROR = -6, /**< Error al leer la llave */
    BLOCK_PBOX_OPEN_FILE_ERROR = -7, /**< Error al abrir el archivo de la P-Box */
    BLOCK_PBOX_GENERATION_ERROR = -8, /**< Error al generar la P-Box */
    BLOCK_PBOX_OUT_OF_THE_RANGE_ERROR = -9, /**< Error con el rango de la P-Box */
    BLOCK_PBOX_REPEATED_VALUES_ERROR = -10, /**< Error con valores repetidos en la P-Box */
} BLOCKStatus;

#endif /* BLOCK_STATUS_H */
