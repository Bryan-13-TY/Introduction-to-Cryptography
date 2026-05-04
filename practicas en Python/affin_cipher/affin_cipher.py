"""Cifrado usando Affin Cipher."""

import numpy as np
import numpy.typing as npt
import ast

from constants import (
    BASE_DIR_CRYPTO,
    RED,
    GREEN,
    YELLOW,
    RESET,
)
from decorators import (
    validate_file,
    validate_key,
)
from utils import (
    clean_console,
    wait_key,
)

__all__ = ["affin_cipher_menu"]

_PRINTABLE_ASCII_LENGHT = 95
_VOCALES_ACENTUDADAS = "ÁÉÍÓÚáéíóú"

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


def _coprime_numbers(n: int) -> npt.NDArray[np.int_]:
    """
    Crea una lista de los números que son coprimos con `n`.

    :param n: Rango de los caracteres ASCII imprimibles.
    :type n: int
    :return: Lista de coprimos.
    :rtype: npt.NDArray[np.int_]
    """
    return np.array([i for i in range(1, n) if _gcd(i, n) == 1])


def _is_valid_key(key: tuple[int, int]) -> bool:
    """
    Verifica que la llave cumpla con las condiciones para el Affin
    Cipher.
    """
    a, b = key
    a_condition = a in _coprime_numbers(_PRINTABLE_ASCII_LENGHT)
    b_condition = 0 <= b <= _PRINTABLE_ASCII_LENGHT - 1

    return a_condition and b_condition


def _get_multiplicative_inverse(n: int, a: int) -> int:
    """
    Devuelve el inverso multiplicativo `b` perteneciente al conjunto
    de los coprimos con `n`, tal que `(a * b) % n = 1`.
    """
    for coprime in _coprime_numbers(n):
        if (a * coprime) % n != 1:
            continue
        
        return coprime
    
    raise RuntimeError(
        f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
        ": No se encontro inverso, pero debía existir"
    )
    

def _key_generator_affin() -> None:
    """Genera una llave válida para el Affin Cipher."""
    rng = np.random.default_rng()
    a = rng.choice(_coprime_numbers(_PRINTABLE_ASCII_LENGHT))
    b = rng.integers(0, _PRINTABLE_ASCII_LENGHT)

    key = np.array([a, b])
    print(f"\n{YELLOW}>>{RESET} Tu llave es K = {key}")


@validate_key(_is_valid_key)
@validate_file("plaintext_file")
def _encrypt_affin(
        key: tuple[int, int],
        plaintext_file: str,
        ciphertext_file: str,
    ) -> None:
    """
    Cifra un texto usando Affin Cipher y lo guarda en un archivo de
    texto.

    :param key: Una llave válida.
    :type key: tuple[int, int]
    :param plaintext_file: Archivo con el texto a cifrar.
    :type plaintext_file: str
    :param ciphertext_file: Archivo con el texto descifrado.
    :type ciphertext_file: str
    """
    with open(BASE_DIR_CRYPTO / plaintext_file, "r", encoding="utf-8") as f:
        plaintext = f.read()
    
    ciphertext = ""
    a, b = key

    for m in plaintext:
        if m in ("\n", "\t") or m in _VOCALES_ACENTUDADAS:
            ciphertext += m  # Solo concatenar
        else:
            c = (a * _get_unicode(m) + b) % _PRINTABLE_ASCII_LENGHT
            ciphertext += _get_char(c)

    with open(BASE_DIR_CRYPTO / ciphertext_file, "w", encoding="utf-8") as f:
        f.write(ciphertext)

    print(
        f"\n{YELLOW}>>{RESET} {GREEN}"
        f"Texto cifrado correctamente y guardado en '{ciphertext_file}'{RESET}"
    )
    

@validate_key(_is_valid_key)
@validate_file("ciphertext_file")
def _decrypt_affin(
        key: tuple[int, int],
        ciphertext_file: str,
    ) -> None:
    """
    Descifra un texto usando Affin Cipher e imprime el resultado.

    :param key: Una llave válida.
    :type key: tuple[int, int]
    :param ciphertext_file: Archivo con el texto descifrado.
    :type ciphertext_file: str
    """
    with open(BASE_DIR_CRYPTO / ciphertext_file, "r", encoding="utf-8") as f:
        ciphertext = f.read()
    
    plaintext = ""
    a, b = key
    a_inverse = _get_multiplicative_inverse(_PRINTABLE_ASCII_LENGHT, a)

    for c in ciphertext:
        if c in("\n", "\t") or c in _VOCALES_ACENTUDADAS:
            plaintext += c  # Solo concatenar
        else:
            m = ((_get_unicode(c) - b) * a_inverse) % _PRINTABLE_ASCII_LENGHT
            plaintext += _get_char(m)

    print(f"\n{YELLOW}>>{RESET} El inverso multiplicativo usado fue: {a_inverse}")
    print(f"{YELLOW}>>{RESET} Texto original recuperado:\n\n{plaintext}")


def affin_cipher_menu() -> None:
    while True:
        clean_console()
        print("""
/*-------------.
| AFFIN CIPHER |              
`-------------*/
              
>> Elija una de las opciones
              
1.- Crear una llave eleatoria válida
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                _key_generator_affin()
                wait_key()
            case "2":
                key = input("\nIngresa una llave válida (ej: (49, 82)): ").strip()
                key_tuple = ast.literal_eval(key)
                
                plaintext_filename = input("Escribe el nombre del archivo con el 'plaintext': ")
                ciphertext_filename = input(
                    "Escribe el nombre del archivo donde se almacenará el ciphertext: "
                )
                _encrypt_affin(key_tuple, plaintext_filename, ciphertext_filename)
                wait_key()
            case "3":
                key = input("\nIngresa una llave válida (ej: (49, 82)): ").strip()
                key_tuple = ast.literal_eval(key)

                ciphertext_filename = input("Escribe el nombre del archivo con el 'ciphertext': ")
                _decrypt_affin(key_tuple, ciphertext_filename)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Opción no válida")
                wait_key()


def main() -> None:
    affin_cipher_menu()

if __name__ == "__main__":
    main()
