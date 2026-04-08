import numpy as np
import numpy.typing as npt
import os
import msvcrt
import ast
import inspect
from pathlib import Path
from typing import (
    Callable,
    ParamSpec,
    TypeVar,
)
from functools import wraps

P = ParamSpec("P")
T = TypeVar("T")

_BASE_DIR = Path(__file__).parent
_PRINTABLE_ASCII_LENGHT = 95
_VOCALES_ACENTUDADAS = "ÁÉÍÓÚáéíóú"

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(_BASE_DIR / filename).exists()


def validate_file(textfile_param_name: str) -> Callable[[Callable[P, T]], Callable[P, T | None]]:
    def _decorator(func: Callable[P, T]) -> Callable[P, T | None]:
        @wraps(func)
        def _wrapper(*args: P.args, **kwargs: P.kwargs) -> T | None:
            try:
                bound = inspect.signature(func).bind(*args, **kwargs)
                bound.apply_defaults()
            except TypeError as e:
                print(f">> Error en argumentos: {e}")
                return None
            
            filename = bound.arguments.get(textfile_param_name)
            if filename is None:
                print(f">> No se encontró el parámetro '{textfile_param_name}'")
                return None
            
            if not isinstance(filename, str):
                print(f">> El parámetro '{textfile_param_name}' debe ser un string")
                return None
            
            if not _file_exists(filename):
                print(f">> El archivo '{filename}' no existe")
                return None
            
            return func(*args, **kwargs)
        return _wrapper
    return _decorator


def _get_unicode(char: str) -> int:
    return ord(char) - 32


def _get_char(unicode: int) -> str:
    return chr(unicode + 32)


def _gcd(a: int, b: int) -> int:
    a, b = abs(a), abs(b)
    while b != 0:
        a, b = b, a % b
    return a


def _coprime_numbers(n: int) -> npt.NDArray[np.int_]:    
    return np.array([i for i in range(1, n) if _gcd(i, n) == 1])


def _is_valid_key(key: tuple[int, int]) -> bool:
    a, b = key
    a_condition = a in _coprime_numbers(_PRINTABLE_ASCII_LENGHT)
    b_condition = 0 <= b <= _PRINTABLE_ASCII_LENGHT - 1

    return a_condition and b_condition


def validate_key(func: Callable[P, T]) -> Callable[P, T | None]:
    def _wrapper(*args, **kwargs) -> T | None:
        try:
            bound = inspect.signature(func).bind(*args, **kwargs)
            bound.apply_defaults()
        except ValueError as e:
            print(f">> Error en argumentos: {e}")
            return None
        
        key = bound.arguments["key"]
        if not _is_valid_key(key):
            print(">> La llave no es válida")
            return
        
        return func(*args, **kwargs)
    return _wrapper


def _get_multiplicative_inverse(n: int, a: int) -> int:
    for coprime in _coprime_numbers(n):
        if (a * coprime) % n != 1:
            continue
        
        return coprime
    
    raise RuntimeError(">> No se encontro inverso, pero debía existir")
    

def key_generator_affin() -> None:
    rng = np.random.default_rng()
    a = rng.choice(_coprime_numbers(_PRINTABLE_ASCII_LENGHT))
    b = rng.integers(0, _PRINTABLE_ASCII_LENGHT)

    key = np.array([a, b])
    print(f"\nTu llave es K = {key}")


@validate_key
@validate_file("plaintext_file")
def encrypt_affin(
        key: tuple[int, int],
        plaintext_file: str,
        ciphertext_file: str,
    ) -> None:
    try:
        with open(_BASE_DIR / plaintext_file, "r", encoding="utf-8") as f:
            plaintext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'plaintext' no existe")
        return
    
    ciphertext = ""
    a, b = key

    for m in plaintext:
        if m in ("\n", "\t") or m in _VOCALES_ACENTUDADAS:
            ciphertext += m  # Solo concatenar
        else:
            c = (a * _get_unicode(m) + b) % _PRINTABLE_ASCII_LENGHT
            ciphertext += _get_char(c)

    try:
        with open(_BASE_DIR / ciphertext_file, "w", encoding="utf-8") as f:
            f.write(ciphertext)
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    

@validate_key
@validate_file("ciphertext_file")
def decrypt_affin(
        key: tuple[int, int],
        ciphertext_file: str,
    ) -> None:
    try:
        with open(_BASE_DIR / ciphertext_file, "r", encoding="utf-8") as f:
            ciphertext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    
    plaintext = ""
    a, b = key
    a_inverse = _get_multiplicative_inverse(_PRINTABLE_ASCII_LENGHT, a)

    for c in ciphertext:
        if c in("\n", "\t") or c in _VOCALES_ACENTUDADAS:
            plaintext += c  # Solo concatenar
        else:
            m = ((_get_unicode(c) - b) * a_inverse) % _PRINTABLE_ASCII_LENGHT
            plaintext += _get_char(m)

    print(f"\n>> El inverso multiplicativo usando fue: {a_inverse}")
    print(f">> Texto original recuperado:\n\n{plaintext}")


def main() -> None:
    while True:
        _clean_console()
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
                key_generator_affin()
                _wait_key()
            case "2":
                key = input("\nIngresa una llave válida (ej: (49, 82)): ").strip()
                key_tuple = ast.literal_eval(key)
                
                plaintext_filename = input("Escribe el nombre del archivo con el 'plaintext': ")
                ciphertext_filename = input(
                    "Escribe el nombre del archivo donde quieres guardar el ciphertext: "
                )
                encrypt_affin(key_tuple, plaintext_filename, ciphertext_filename)
                _wait_key()
            case "3":
                key = input("\nIngresa una llave válida (ej: (49, 82)): ").strip()
                key_tuple = ast.literal_eval(key)

                ciphertext_filename = input("Escribe el nombre del archivo con el 'ciphertext': ")
                
                decrypt_affin(key_tuple, ciphertext_filename)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
