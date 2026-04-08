"""Cifrado usando DES."""

import base64
import msvcrt
import os
from pathlib import Path
from typing import (
    Callable,
    TypeVar,
    ParamSpec,
)
from functools import wraps
import inspect

from Crypto.Random import get_random_bytes
from Crypto.Cipher import DES
from Crypto.Util.Padding import (
    pad,
    unpad,
)

__all__ = [
    "random_key_generator",
    "encrypt_file",
    "decrypt_file",
    "validate_file",
    "validate_key",  
]

P = ParamSpec("P")
T = TypeVar("T")

_BASE_DIR = Path(__file__).parent

def _clean_console() -> None:
    os.system("cls" if os.name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(_BASE_DIR / filename).exists()


def _is_valid_file_size(filename: str) -> bool:
    """
    Varifica si el tamaño de un archivo es de mínimo 100 KB.

    :param filename: Nombre del archivo.
    :type filename: str
    :return: True si tiene como mínimo 100 KB de tamaño, False en caso
             contrario.
    :rtype: bool
    """
    size = os.path.getsize(_BASE_DIR / filename)
    return size > 100 * 1024


def validate_file(textfile_param_name: str) -> Callable[[Callable[P, T]], Callable[P, T | None]]:
    """
    Crea un decorador que valida un archivo recibido como argumento
    en la función decorada.

    El decorador verifica que el parámetro especificado:
    - Existe dentro de los argumentos de la función.
    - Sea de tipo `str`.
    - Corresponda a un archivo existente.
    - Cumpla con un tamaño requerido.

    Usa `inspect.signature().bind()` para mapear `*args` y `**kwargs`
    a los nombres de parámetros de la función.

    Si alguna validación falla, se imprime un mensaje de error y la
    función decorada no se ejecuta, retornando `None`.

    :param textfile_param_name: Nombre del parámetro en la función
                                decorada que representa el archivo
                                a validar.
    :type textfile_param_name: str
    :return: Un decorador que envuelve la función original agregando
             validaciones sobre el archivo indicado.
    :rtype: Callable[[Callable[P, T]], Callable[P, T | None]]
    """
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
            
            if not _is_valid_file_size(filename):
                print(">> El archivo debe ser mayor a 100 KB")
                return None
            
            return func(*args, **kwargs)
        return _wrapper
    return _decorator
            

def _is_valid_key(key: bytes) -> bool:
    """
    Verifica que la llave en base 64 sea de 8 bytes.

    :param key: Llave en base 64.
    :type key: bytes
    :return: True si es de 8 bytes, False en caso contrario.
    :rtype: bool
    """
    return len(key) == 8


def validate_key(func: Callable[P, T]) -> Callable[P, T | None]:
    """
    Decorador que valida el tamaño de una llave para DES.

    Si la validación falla, se imprime un mensaje de error y la función
    decorada no se ejecuta, retornando `None`. 

    :param func: Función a decorar.
    :type func: Callable[[Callable[P, T]], Callable[P, T | None]]
    :return: Función envuelta que retorna el resultado original o
             `None` si la validación falla.
    :rtype: Callable[[Callable[P, T]], Callable[P, T | None]]
    """
    def _wrapper(*args: P.args, **kwargs: P.kwargs) -> T | None:
        try:
            bound = inspect.signature(func).bind(*args, **kwargs)
            bound.apply_defaults()
        except TypeError as e:
            print(f">> Error en argumentos: {e}")
            return None
        
        key = bound.arguments["key"]

        if not _is_valid_key(key):
            print(">> La llave DES debe tener exactamente 8 bytes (64 bits)")
            return None
        
        return func(*args, **kwargs)
    return _wrapper


def base64_to_binary(base64_str: str) -> str:
    decoded = base64.b64decode(base64_str)
    binary = "".join(format(byte, "08b") for byte in decoded)
    return binary


def binary_to_base64(binary_str: str) -> str:
    data = int(binary_str, 2).to_bytes((len(binary_str) + 7) // 8, "big")
    encode = base64.b64encode(data)
    return encode.decode()


def random_key_generator() -> bytes:
    """
    Genera una llave en bse 64 de 8 bytes.
    
    :return: Llave en base 64 de 8 bytes.
    :rtype: bytes
    """
    key = get_random_bytes(8)
    key_base64 = base64.b64encode(key).decode()
    print(f"\nLlave DES (Base64): {key_base64}")
    return key


@validate_key
@validate_file("plaintext_file")
def encrypt_file(key: bytes, plaintext_file: str, ciphertext_file: str) -> None:
    """
    Cifra un archivo de texto usando DES e IV (Initialization Vector)
    de 8 bytes para hacer el cifrado no determinista.

    Después del cifrafo se creará un archivo de texto con el resultado
    del cifrado `ciphertext_file`.

    :param key: Llave en base 64 de 8 bytes.
    :type key: bytes
    :param plaintext_file: Archivo de texto con el plaintext.
    :type plaintext_file: str
    :param ciphertext_file: Archivo de texto con el ciphertext.
    :type ciphertext_file: str
    """
    with open(_BASE_DIR / plaintext_file, "rb") as f:
        data = f.read()

    iv = get_random_bytes(8)  # Bloque de 8 bytes para DES
    cipher = DES.new(key, DES.MODE_CBC, iv=iv)

    ciphertext = cipher.encrypt(pad(data, 8))

    with open(_BASE_DIR / ciphertext_file, "wb") as f:
        f.write(base64.b64encode(iv + ciphertext))

    print("Archivo encriptado")


@validate_key
@validate_file("ciphertext_file")
def decrypt_file(key: bytes, ciphertext_file: str, output_file: str) -> None:
    """
    Descifra un archivo de texto usando DES e IV (Initialization Vector)
    de 8 bytes.

    Después del descifrado se creará un archivo de texto con el
    resultado del descifrado `output_file`.

    :param key: Llave en base 64 de 8 bytes.
    :type key: bytes
    :param ciphertext_file: Archivo de texto con el ciphertext.
    :type ciphertext_file: str
    :param output_file: Archivo original recuperado después del
                        descifrado.
    :type output_file: str
    """
    with open(_BASE_DIR / ciphertext_file, "rb") as f:
        data = base64.b64decode(f.read())

    iv = data[:8]
    ciphertext = data[8:]

    cipher = DES.new(key, DES.MODE_CBC, iv=iv)
    plaintext = unpad(cipher.decrypt(ciphertext), 8)

    with open(_BASE_DIR / output_file, "wb") as f:
        f.write(plaintext)
        
    print("Archivo desencriptado")


def main() -> None:
    while True:
        _clean_console()
        print("""
/*-------------.
| BLOCK CIPHER |
`-------------*/
              
>> Elija una de las opciones
              
1.- Crear una llave para DES 
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                random_key_generator()
                _wait_key()
            case "2":
                key_base64 = input("\nEscribe la llave en base 64: ")
                try:
                    key = base64.b64decode(key_base64)
                except Exception:
                    print(">> La llave en base64 no es válida")
                    _wait_key()
                    continue

                infile = input("Escribe el nombre del archivo con el 'plaintext': ")
                outfile = input(
                    "Escribe el nombre del archivo donde se almacenará el 'ciphertext': "
                )

                encrypt_file(key, infile, outfile)
                _wait_key()
            case "3":
                key_base64 = input("\nEscribe la llave en base 64: ")
                try:
                    key = base64.b64decode(key_base64)
                except Exception:
                    print(">> La llave en base64 no es válida")
                    _wait_key()
                    continue

                infile = input("Escribe el nombre del archivo con el 'ciphertext': ")                
                outfile = input(
                    "Escribe el nombre del archivo donde se almacenará el 'plaintext': "
                )
                
                decrypt_file(key, infile, outfile)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
