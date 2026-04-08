"""Cifrado usando Permutation Cipher."""

import numpy as np
import numpy.typing as npt
from pathlib import Path
import msvcrt
import os
from typing import (
    Callable,
    TypeVar,
    ParamSpec,
)
from functools import wraps

__all__ = [
    "permutation_random_generator",
    "encrypt_permutation",
    "decrypt_permutation",
    "validate_files",
]

P = ParamSpec("P")
T = TypeVar("T")
_BASE_DIR = Path(__file__).parent

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(_BASE_DIR / filename).exists()


def validate_files(func: Callable[P, T]) -> Callable[P, T | None]:
    """
    Decorador que valida que los argumentos de tipo `str` correspondan
    a archivos existentes.

    Recorre los argumentos posicionales de la función decorada y
    verifica si cada uno representa un archivo válido en el sistema.
    Si alguno no existe, imprime un mensaje de error y al función
    decorada no se ejecuta, retornando `None`.

    :param func: Función a decorar.
    :type func: Callable[[Callable[P, T]], Callable[P, T | None]]
    :return: Función envuelta que retorna el resultado original o
             `None` si la validación falla.
    :rtype: Callable[[Callable[P, T]], Callable[P, T | None]]
    """
    @wraps(func)
    def _wrapper(*args: P.args, **kwargs: P.kwargs) -> T | None:
        for arg in args:
            if isinstance(arg, str) and not _file_exists(arg):
                print(f">> El archivo '{arg}' no existe")
                return None
            
        return func(*args, **kwargs)
    return _wrapper


def _convert_permutation_to_string(permutation: npt.NDArray[np.int_]) -> str:
    """
    Convierte una permutación como array a un string.

    :param permutation: Permutación como array.
    :type permutation: npt.NDArray[np.int_]
    :return: Permutación como string.
    :rtype: str
    """
    return " ".join(map(str, permutation))


def _convert_permutation_to_array(str_permutation: str) -> npt.NDArray[np.int_]:
    """
    Convierte una permutación como string a un array.

    :param str_permutation: Permutación como string.
    :type str_permutation: str
    :return: Permutación como array.
    :rtype: npt.NDArray[np.int_]
    """
    return np.array([int(p) for p in str_permutation.split()], dtype=np.int_)


def _recover_permutation_from_file(permutation_file: str) -> npt.NDArray[np.int_]:
    """
    Carga una permutación desde un archivo de texto.

    :param permutation_file: Archivo de texto con la permutación.
    :type permutation_file: str
    :return: Permutación como array.
    :rtype: npt.NDArray[np.int_]
    """
    with open(_BASE_DIR / permutation_file, "r", encoding="utf-8") as f:
        permutation = f.read()

    return _convert_permutation_to_array(permutation)


def _save_permutation_in_file(permutation: npt.NDArray[np.int_]) -> None:
    """
    Guarda una permutación en un archivo de texto. En el nombre del
    archivo se indica el tamaño de la permutación como sigue
    'permutation_SIZE.txt'.

    :param permutation: Permutación como array.
    :type permutation: npt.NDArray[np.int_]
    """
    str_permutation = _convert_permutation_to_string(permutation)

    with open(_BASE_DIR / f"permutation_{len(permutation)}.txt", "w", encoding="utf-8") as f:
        f.write(str_permutation)


def _permutation_generator(permutation_size: int) -> npt.NDArray[np.int_]:
    """
    Devuelve una permutación aleatoria de tamaño `permutation_size`.

    :param permutation_size: Tamaño de la permutación.
    :type permutation_size: int
    :return: Permutación como array.
    :rtype: npt.NDArray[np.int_]
    """
    return np.random.permutation(np.arange(1, permutation_size + 1))


def _inverse_permutation_generator(
        random_permutation: npt.NDArray[np.int_]
    ) -> npt.NDArray[np.int_]:
    """
    Devuelve la permutación inversa.

    :param random_permutation: Permutación como array.
    :type random_permutation: npt.NDArray[np.int_]
    :return: Permutación inversa como array.
    :rtype: npt.NDArray[np.int_]
    """
    inverse = np.empty_like(random_permutation)

    for i, p in enumerate(random_permutation):
        inverse[p - 1] = i + 1

    return inverse


def permutation_random_generator(
        permutation_size: int
    ) -> tuple[npt.NDArray[np.int_], npt.NDArray[np.int_]]:
    """
    Genera una permutación aleatoria y su inversa de tamaño `permutation_size`.

    :param permutation_size: Tamaño de la permutación.
    :type permutation_size: int
    :return: Permutación aleatoria y su inversa.
    :rtype: tuple[npt.NDArray[np.int_], npt.NDArray[np.int_]
    """
    permutation = _permutation_generator(permutation_size)
    inverse_permutation = _inverse_permutation_generator(permutation)
    _save_permutation_in_file(permutation)
    return permutation, inverse_permutation

@validate_files
def encrypt_permutation(file_plaintext: str, permutation_file: str) -> None:
    """
    Cifra un texto usando Permutation Cipher y lo guarda en un archivo
    de texto. Toma el texto de un archivo de texto al igual que la
    permutación a usar para el cifrado.

    :param file_plaintext: Archivo con el texto a cifrar.
    :type file_plaintext: str
    :param permutation_file: Archivo con la permutación.
    :type permutation_file: str 
    """
    with open(_BASE_DIR / file_plaintext, "r", encoding="utf-8") as f:
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

    with open(_BASE_DIR / f"{size_plaintext}_ciphertext.txt", "w", encoding="utf-8") as f:
        f.write(ciphertext)
    
    print(f"\nEl texto cifrado es el siguiente:\n\n{ciphertext}")

@validate_files
def decrypt_permutation(file_ciphertext: str, permutation_file: str) -> None:
    """
    Descifra un texto usando Permutation Cipher e imprime el resultado.
    Toma el texto de un archivo de texto al igual que la permutación
    a usar para el descifrado.

    :param file_ciphertext: Archivo con el texto a descifrar.
    :type file_ciphertext: str
    :param permutation_file: Archivo con la permutación.
    :type permutation_file: str 
    """
    original_size = int(file_ciphertext.split("_")[0])

    with open(_BASE_DIR / file_ciphertext, "r", encoding="utf-8") as f:
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

    print(f"\nEl texto original recuperado es el siguiente:\n\n{plaintext[:original_size]}")


def main() -> None:
    while True:
        _clean_console()
        print("""
/*-------------------.
| PERMUTATION CIPHER |
`-------------------*/
              
>> Elija una de las opciones
              
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
                    print(">> Debe ser un número")
                    _wait_key()
                    continue
                if not int(permutation_size) >= 3:
                    print(">> Deber ser mayor o igual a 3")
                    _wait_key()
                    continue
                
                (
                    permutation,
                    inverse_permutation
                ) = permutation_random_generator(int(permutation_size))

                print(f"\nLa permutación generada es π(x):\n\n", permutation)
                print(f"\nY su inversa π^-1(x):\n\n", inverse_permutation)               
                _wait_key()
            case "2":
                file_plaintext = input("\nIngresa el nombre del archivo con el 'plaintext' M: ")
                file_permutation = input(
                    "Escribe el nombre del archivo con la permutación a usar: "
                )
                
                encrypt_permutation(file_plaintext, file_permutation)
                _wait_key()
                pass
            case "3":
                file_ciphertext = input("\nIngresa el nombre del archivo con el 'ciphertext' C: ")
                file_permutation = input(
                    "Escribe el nombre del archivo con la permutación a usar: "
                )

                decrypt_permutation(file_ciphertext, file_permutation)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
