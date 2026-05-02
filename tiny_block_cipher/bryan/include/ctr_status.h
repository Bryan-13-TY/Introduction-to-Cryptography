#ifndef CTR_STATUS_H
#define CTR_STATUS_H

/**
 * @enum CTRStatus
 * @brief Códigos para el modo de opración CTR.
 * 
 * Las funciones para generar una S-Box, P-Box, llave secreta,
 * cifrar y descifrar devuelven uno de estos valores para
 * indicar éxito o el tipo de error ocurrido.
 * 
 */
typedef enum {
    CTR_OK = 0, /**< Operación exitosa */
    CTR_SBOX_OPEN_FILE_ERROR = -1, /**< Error al abrir el archivo de la S-Box */
    CTR_SBOX_GENERATION_ERROR = -2, /**< Error al generar la S-Box */
    CTR_SBOX_MEMORY_ERROR = -3, /**< Error al reservar memoria para la S-Box */
    CTR_KEY_OPEN_FILE_ERROR = -4, /**< Error al abrir el archivo de la llave */
    CTR_KEY_GENERATION_ERROR = -5, /**< Error al generar la llave */
    CTR_KEY_READ_ERROR = -6, /**< Error al leer la llave */
    CTR_PBOX_OPEN_FILE_ERROR = -7, /**< Error al abrir el archivo de la P-Box */
    CTR_PBOX_GENERATION_ERROR = -8, /**< Error al generar la P-Box */
    CTR_PBOX_OUT_OF_THE_RANGE_ERROR = -9, /**< Error con el rango de la P-Box */
    CTR_PBOX_REPEATED_VALUES_ERROR = -10, /**< Error con valores repetidos en la P-Box */
} CTRStatus;

#endif /* CTR_STATUS_H */