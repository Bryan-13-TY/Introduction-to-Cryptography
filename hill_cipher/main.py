"""Cifrado usando Hill Cipher."""

import numpy as np
import numpy.typing as npt
import msvcrt
import os
import inspect
from pathlib import Path
from functools import wraps
from typing import (
    Callable,
    ParamSpec,
    TypeVar,
)

__all__ = [
    "key_generator_hill",
    "encrypt_hill",
    "decrypt_hill"
]

_BASE_DIR = Path(__file__).parent
_PRINTABLE_ASCII_LENGHT = 95

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(BASE_DIR / filename).exists()


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


def _generate_random_matrix() -> np.ndarray:
    return np.random.randint(low=0, high=_PRINTABLE_ASCII_LENGHT, size=(2,2))


def _calculate_determinant(matrix: np.ndarray) -> int:
    a, b = matrix[0, 0], matrix[0, 1]
    c, d = matrix[1, 0], matrix[1, 1]
    return int(a * d - b * c)


def _calculate_adjuntate_matrix(matrix: np.ndarray) -> np.ndarray:
    a, b = matrix[0, 0], matrix[0, 1]
    c, d = matrix[1, 0], matrix[1, 1]
    return np.array([[d, -b], [-c, a]])


def key_generator_hill() -> np.ndarray:
    while True:
        key = _generate_random_matrix()
        determinant = _calculate_determinant(key)
        if _gcd(determinant, _PRINTABLE_ASCII_LENGHT) == 1:
            print(f"\nTu llave es K =\n{key}")
            return key
        

def _calculate_inverse_key(key: np.ndarray) -> np.ndarray | None:
    key_determinant = _calculate_determinant(key) % _PRINTABLE_ASCII_LENGHT
    inverse_key_determinant = _multiplicative_inverse(_PRINTABLE_ASCII_LENGHT, key_determinant) 

    if inverse_key_determinant is None:
        return None
    
    adjuntate_key = _calculate_adjuntate_matrix(key)
    inverse_key = (inverse_key_determinant * adjuntate_key) % _PRINTABLE_ASCII_LENGHT
    return inverse_key


def _convert_key_format(key: str) -> np.ndarray:
    key_list = key.split(", ")
    key_matrix = [
        [int(key_list[0]), int(key_list[1])],
        [int(key_list[2]), int(key_list[3])]
    ]

    return np.array(key_matrix)
