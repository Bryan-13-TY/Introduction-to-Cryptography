"""Cifrado usando Hill Cipher."""

import numpy as np
import numpy.typing as npt
import msvcrt
import os
from typing import TypeAlias

from constants import BASE_DIR_CRYPTO
from decorators import (
    validate_file,
    validate_key,
)

__all__ = ["hill_cipher_menu"]

Matrix2x2: TypeAlias = npt.NDArray[np.int_]

_PRINTABLE_ASCII_LENGHT = 95

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _get_unicode(char: str) -> int:
    return ord(char) - 32


def _get_char(unicode: int) -> str:
    return chr(unicode + 32)


def _gcd(a: int, b: int) -> int:
    """Máximo común divisor usando el algoritmo extendido de Euclides."""
    a, b = abs(a), abs(b)
    while b != 0:
        a, b = b, a % b
    return a


def _multiplicative_inverse(n: int, a: int) -> int | None:
    a = a % n
    a0 = n
    b0 = a
    t0 = 0
    t = 1
    q = a0 // b0
    r = a0 - q * b0
    while r > 0:
        temp = (t0 - q * t) % n
        t0 = t
        t = temp
        a0 = b0
        b0 = r
        q = a0 // b0
        r = a0 - q * b0
    
    if not b0 == 1:
        print(f">> {a} no tiene inverso multiplicativo módulo {n}")
        return None
    
    return t % n


def _generate_random_matrix() -> Matrix2x2:
    return np.random.randint(low=0, high=_PRINTABLE_ASCII_LENGHT, size=(2,2), dtype=np.int_)


def _calculate_determinant(matrix: Matrix2x2) -> int:
    a, b = matrix[0, 0], matrix[0, 1]
    c, d = matrix[1, 0], matrix[1, 1]
    
    return int(a * d - b * c)


def _is_valid_key(key: Matrix2x2) -> bool:
    """
    Determina si una llave para Hill Cipher es valida cumpliendo con
    gcd(key, n) = 1.
    """
    return _gcd(
        _calculate_determinant(key),
        _PRINTABLE_ASCII_LENGHT,
    ) == 1


def _calculate_adjuntate_matrix(matrix: Matrix2x2) -> Matrix2x2:
    a, b = matrix[0, 0], matrix[0, 1]
    c, d = matrix[1, 0], matrix[1, 1]

    return np.array([[d, -b], [-c, a]], dtype=np.int_)


def _key_generator_hill() -> Matrix2x2:
    while True:
        key = _generate_random_matrix()
        determinant = _calculate_determinant(key)
        if _gcd(determinant, _PRINTABLE_ASCII_LENGHT) == 1:
            print(f"\nTu llave es K =\n{key}")
            return key


def _calculate_inverse_key(key: Matrix2x2) -> Matrix2x2 | None:
    key_determinant = _calculate_determinant(key) % _PRINTABLE_ASCII_LENGHT
    
    inverse_key_determinant = _multiplicative_inverse(
        _PRINTABLE_ASCII_LENGHT,
        key_determinant
    ) 

    if inverse_key_determinant is None:
        return None
    
    adjuntate_key = _calculate_adjuntate_matrix(key)
    inverse_key = (
        inverse_key_determinant * adjuntate_key
    ) % _PRINTABLE_ASCII_LENGHT
    
    return inverse_key.astype(np.int_)


def _convert_key_format(key: str) -> Matrix2x2:
    key_list = key.split(", ")
    
    key_matrix = [
        [int(key_list[0]), int(key_list[1])],
        [int(key_list[2]), int(key_list[3])]
    ]

    return np.array(key_matrix, dtype=np.int_)


@validate_key(_is_valid_key)
@validate_file("plaintext_file")
def _encrypt_hill(
        key: Matrix2x2,
        plaintext_file: str,
        ciphertext_file: str,
    ) -> None:
    try:
        with open(BASE_DIR_CRYPTO / plaintext_file, "r", encoding="utf-8") as f:
            plaintext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'plaintext' no existe")
        return
    
    # Filtrar caracteres que sí se cifran
    filtered_text = [c for c in plaintext if 32 <= ord(c) <= 126]

    # Padding con X, para asegurar la división en bloques de 2
    if len(filtered_text) % 2 != 0:
        filtered_text.append("X")

    # === CIFRADO EN BLOQUES ===
    encrypted_clean = ""
    i = 0
    while i < len(filtered_text):
        block = filtered_text[i:i+2]
        vector = np.array([_get_unicode(block[0]), _get_unicode(block[1])], dtype=(np.int_))
        cipher = np.dot(key, vector) % _PRINTABLE_ASCII_LENGHT
        encrypted_clean += _get_char(cipher[0]) + _get_char(cipher[1])
        i += 2

    # === REINSERCIÓN CORRECTA ===
    final_ciphertext = ""
    k = 0  # Índice del texto cifrado limpio

    for c in plaintext:
        if 32 <= ord(c) <= 126:
            final_ciphertext += encrypted_clean[k]
            k += 1
        else:
            final_ciphertext += c

    if k < len(encrypted_clean):
        final_ciphertext += encrypted_clean[k:]

    try:
        with open(BASE_DIR_CRYPTO / ciphertext_file, "w", encoding="utf-8") as f:
            f.write(final_ciphertext)
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    

@validate_key(_is_valid_key)
@validate_file("ciphertext_file")
def _decrypt_hill(
        key: Matrix2x2,
        ciphertext_file: str,
    ) -> None:
    try:
        with open(BASE_DIR_CRYPTO / ciphertext_file, "r", encoding="utf-8") as f:
            ciphertext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    
    inverse_key = _calculate_inverse_key(key)

    # Filtrar caracteres cifrados válidos
    filtered_text = [c for c in ciphertext if 32 <= ord(c) <= 126]

    # === DESCIFRADO POR BLOUES ===
    decrypted_clean = ""
    i = 0
    while i < len(filtered_text):
        block = filtered_text[i:i+2]
        vector = np.array([_get_unicode(block[0]), _get_unicode(block[1])])
        plain = np.dot(inverse_key, vector) % _PRINTABLE_ASCII_LENGHT
        decrypted_clean += _get_char(plain[0]) + _get_char(plain[1])
        i += 2

    # === REINSERCIÓN CORRECTA ===
    final_plaintext = ""
    k = 0  # Índice del texto original limpio

    for c in ciphertext:
        if 32 <= ord(c) <= 126:
            final_plaintext += decrypted_clean[k]
            k += 1
        else:
            final_plaintext += c

    # Eliminar el padding si es que existe
    if final_plaintext.endswith("X"):
        final_plaintext = final_plaintext[:-1]

    print(f"\n>> El K^-1 mod n usado fue: \n{_calculate_inverse_key(key)}")
    print(f">> Texto original recuperado:\n\n{final_plaintext}")


def hill_cipher_menu() -> None:
    while True:
        _clean_console()
        print("""
/*------------.
| HILL CIPHER |
`------------*/
              
>> Elija una de las opciones
              
1.- Crear una llave eleatoria válida
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                key = _key_generator_hill()
                print(f"\nY su inversa K^-1 =\n{_calculate_inverse_key(key)}")
                _wait_key()
            case "2":
                key = input("\nIngresa una llave válida (ej: [81, 63, 66, 85]): ").strip()
                key_valid_format = _convert_key_format(key)
                
                plaintext_filename = input("Escribe el nombre del archivo con el 'plaintext': ")        
                ciphertext_filename = input(
                    "Escribe el nombre del archivo donde quieres guardar el ciphertext: "
                )
                _encrypt_hill(key_valid_format, plaintext_filename, ciphertext_filename)
                _wait_key()
            case "3":
                key = input("\nIngresa una llave válida (ej: [81, 63, 66, 85]): ").strip()
                key_valid_format = _convert_key_format(key)

                ciphertext_filename = input("Escribe el nombre del archivo con el 'ciphertext': ")
                _decrypt_hill(key_valid_format, ciphertext_filename)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()    


def main() -> None:
    hill_cipher_menu()

if __name__ == "__main__":
    main()
