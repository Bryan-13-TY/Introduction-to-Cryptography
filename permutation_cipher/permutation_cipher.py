"""Cifrado usando Permutation Cipher."""

import numpy as np
import numpy.typing as npt
from typing import TypeAlias

from constants import (
    BASE_DIR_CRYPTO,
    RED,
    GREEN,
    YELLOW,
    RESET,
)
from decorators import validate_files
from utils import (
    clean_console,
    wait_key,
)

__all__ = ["permutation_cipher_menu"]

Permutation: TypeAlias = npt.NDArray[np.int_]

def _convert_permutation_to_string(permutation: Permutation) -> str:
    """
    Convierte una permutación como array a un string.

    :param permutation: Permutación como array.
    :type permutation: Permutation
    :return: Permutación como string.
    :rtype: str
    """
    return " ".join(map(str, permutation))


def _convert_permutation_to_array(str_permutation: str) -> Permutation:
    """
    Convierte una permutación como string a un array.

    :param str_permutation: Permutación como string.
    :type str_permutation: str
    :return: Permutación como array.
    :rtype: Permutation
    """
    return np.array([int(p) for p in str_permutation.split()], dtype=np.int_)


def _recover_permutation_from_file(permutation_file: str) -> Permutation:
    """
    Carga una permutación desde un archivo de texto.

    :param permutation_file: Archivo de texto con la permutación.
    :type permutation_file: str
    :return: Permutación como array.
    :rtype: Permutation
    """
    with open(BASE_DIR_CRYPTO / permutation_file, "r", encoding="utf-8") as f:
        permutation = f.read()

    return _convert_permutation_to_array(permutation)


def _save_permutation_in_file(permutation: Permutation) -> None:
    """
    Guarda una permutación en un archivo de texto. En el nombre del
    archivo se indica el tamaño de la permutación como sigue
    'permutation_SIZE.txt'.

    :param permutation: Permutación como array.
    :type permutation: Permutation
    """
    str_permutation = _convert_permutation_to_string(permutation)
    permutation_file = f"permutation_{len(permutation)}.txt"

    with open(
        BASE_DIR_CRYPTO / permutation_file,
        "w",
        encoding="utf-8",
    ) as f:
        f.write(str_permutation)
    
    print(
        f"\n{YELLOW}>>{RESET} Permutación generada "
        f"correctamente y guardada en '{permutation_file}'"
    )


def _permutation_generator(permutation_size: int) -> Permutation:
    """
    Devuelve una permutación aleatoria de tamaño `permutation_size`.

    :param permutation_size: Tamaño de la permutación.
    :type permutation_size: int
    :return: Permutación como array.
    :rtype: Permutation
    """
    return np.random.permutation(np.arange(1, permutation_size + 1))


def _inverse_permutation_generator(
        random_permutation: Permutation
    ) -> Permutation:
    """
    Devuelve la permutación inversa.

    :param random_permutation: Permutación como array.
    :type random_permutation: Permutation
    :return: Permutación inversa como array.
    :rtype: Permutation
    """
    inverse = np.empty_like(random_permutation)

    for i, p in enumerate(random_permutation):
        inverse[p - 1] = i + 1

    return inverse


def _permutation_random_generator(
        permutation_size: int
    ) -> tuple[Permutation, Permutation]:
    """
    Genera una permutación aleatoria y su inversa de tamaño `permutation_size`.

    :param permutation_size: Tamaño de la permutación.
    :type permutation_size: int
    :return: Permutación aleatoria y su inversa.
    :rtype: tuple[Permutation, Permutation]
    """
    permutation = _permutation_generator(permutation_size)
    inverse_permutation = _inverse_permutation_generator(permutation)
    _save_permutation_in_file(permutation)
    return permutation, inverse_permutation


@validate_files
def _encrypt_permutation(plaintext_file: str, permutation_file: str) -> None:
    """
    Cifra un texto usando Permutation Cipher y lo guarda en el archivo
    de texto. El texto a cifrar y la permutación a usar se toman de un
    archivo de texto.

    :param file_plaintext: Archivo con el texto a cifrar.
    :type file_plaintext: str
    :param permutation_file: Archivo con la permutación.
    :type permutation_file: str 
    """
    with open(BASE_DIR_CRYPTO / plaintext_file, "r", encoding="utf-8") as f:
        plaintext = f.read()

    permutation = _recover_permutation_from_file(permutation_file)
    block_size = len(permutation)
    
    size_plaintext = len(plaintext)  # Tamaño original del plaintext
    
    # Padding con X, para asegurar la dividión en bloques de 'block_size'
    while len(plaintext) % len(permutation) != 0:
        plaintext += "X"
        
    # Dividir en bloques de 'block_size'
    plaintext_blocks: list[str] = []
    i = 0
    while i < len(plaintext):
        block = plaintext[i:i+block_size]
        plaintext_blocks.append(block)
        i += block_size
   
    # Cifrar bloque por bloque
    aux_plaintext_blocks: list[str] = []
    aux_block = ""
    for block in plaintext_blocks:
        for c in range(block_size):
            index = permutation[c]
            aux_block += block[index - 1]
        
        aux_plaintext_blocks.append(aux_block)
        aux_block = ""

    ciphertext = "".join(aux_plaintext_blocks)
    ciphertext_file = f"{size_plaintext}_ciphertext.txt"

    with open(BASE_DIR_CRYPTO / ciphertext_file, "w", encoding="utf-8") as f:
        f.write(ciphertext)

    print(
        f"\n{YELLOW}>>{RESET} {GREEN}"
        f"Texto cifrado correctamente y guardado en '{ciphertext_file}'{RESET}"
    )


@validate_files
def _decrypt_permutation(ciphertext_file: str, permutation_file: str) -> None:
    """
    Descifra un texto usando Permutation Cipher e imprime el resultado.
    Toma el texto de un archivo de texto al igual que la permutación
    a usar para el descifrado.

    :param file_ciphertext: Archivo con el texto a descifrar.
    :type file_ciphertext: str
    :param permutation_file: Archivo con la permutación.
    :type permutation_file: str 
    """
    original_size = int(ciphertext_file.split("_")[0])

    with open(BASE_DIR_CRYPTO / ciphertext_file, "r", encoding="utf-8") as f:
        ciphertext = f.read()

    permutation = _recover_permutation_from_file(permutation_file)
    inverse_permutation = _inverse_permutation_generator(permutation)
    block_size = len(permutation)

    # Dividir en bloques de 'block_size'
    ciphertext_blocks: list[str] = []
    i = 0
    while i < len(ciphertext):
        block = ciphertext[i:i+block_size]
        ciphertext_blocks.append(block)
        i += block_size

    # Descifrar bloque por bloque
    aux_ciphertex_blocks: list[str] = []
    aux_block = ""
    for block in ciphertext_blocks:
        for c in range(block_size):
            index = inverse_permutation[c]
            aux_block += block[index - 1]

        aux_ciphertex_blocks.append(aux_block)
        aux_block = ""

    plaintext = "".join(aux_ciphertex_blocks)

    print(
        f"\n{YELLOW}>>{RESET} El texto original recuperado es el siguiente:"
        f"\n\n{plaintext[:original_size]}"
    )


def permutation_cipher_menu() -> None:
    while True:
        clean_console()
        print(f"""
/*-------------------.
| PERMUTATION CIPHER |
`-------------------*/
              
{YELLOW}>>{RESET} Elija una de las opciones
              
1.- Crear una permutación π de tamaño n
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                permutation_size = input("\nIngresa el tamaño que tendrá la permutación: ")
                if not permutation_size.isdigit():
                    print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Debe ser un número")
                    wait_key()
                    continue
                if not int(permutation_size) >= 3:
                    print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Deber ser mayor o igual a 3")
                    wait_key()
                    continue
                
                (
                    permutation,
                    inverse_permutation
                ) = _permutation_random_generator(int(permutation_size))

                print(f"\n{YELLOW}>>{RESET} La permutación generada es π(x):\n\n", permutation)
                print(f"\n{YELLOW}>>{RESET} Y su inversa π^-1(x):\n\n", inverse_permutation)               
                wait_key()
            case "2":
                file_plaintext = input("\nIngresa el nombre del archivo con el 'plaintext': ")
                file_permutation = input(
                    "Escribe el nombre del archivo con la permutación a usar: "
                )
                _encrypt_permutation(file_plaintext, file_permutation)
                wait_key()
            case "3":
                file_ciphertext = input("\nIngresa el nombre del archivo con el 'ciphertext': ")
                file_permutation = input(
                    "Escribe el nombre del archivo con la permutación a usar: "
                )
                _decrypt_permutation(file_ciphertext, file_permutation)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Opción no válida")
                wait_key()


def main() -> None:
    permutation_cipher_menu()

if __name__ == "__main__":
    main()
