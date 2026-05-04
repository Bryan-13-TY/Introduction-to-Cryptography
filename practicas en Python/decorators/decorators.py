import inspect
import os
from pathlib import Path
from functools import wraps
from typing import (
    Callable,
    ParamSpec,
    TypeVar,
)

from constants import (
    BASE_DIR_CRYPTO,
    RED,
    YELLOW,
    RESET,
)

__all__ = [
    "validate_files",
    "validate_file_DES",
    "validate_file",
    "validate_key",
]

P = ParamSpec("P")
T = TypeVar("T")
K = TypeVar("K")  # Tipo de la llave

def _file_exists(filename: str) -> bool:
    return Path(BASE_DIR_CRYPTO / filename).exists()


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
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: El archivo '{arg}' no existe")
                return None
        
        return func(*args, **kwargs)
    return _wrapper


def _is_valid_file_size_DES(filename: str) -> bool:
    """
    Varifica si el tamaño de un archivo es de mínimo 100 KB para DES.

    :param filename: Nombre del archivo.
    :type filename: str
    :return: True si tiene como mínimo 100 KB de tamaño, False en caso
             contrario.
    :rtype: bool
    """
    size = os.path.getsize(BASE_DIR_CRYPTO / filename)
    return size > 100 * 1024


def validate_file_DES(
        textfile_param_name: str
    ) -> Callable[[Callable[P, T]], Callable[P, T | None]]:
    """
    Decorador que valida un archivo recibido como argumento
    en las funciones de DES.

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
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET} en argumentos: {e}")
                return None
            
            filename = bound.arguments.get(textfile_param_name)
            if filename is None:
                print(
                    f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                    f": No se encontró el parámetro '{textfile_param_name}'"
                )
                return None
            
            if not isinstance(filename, str):
                print(
                    f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                    f": El parámetro '{textfile_param_name}' debe ser un string"
                )
                return None
            
            if not _file_exists(filename):
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: El archivo '{filename}' no existe")
                return None
            
            if not _is_valid_file_size_DES(filename):
                print(
                    f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                    f": El archivo '{filename}' debe pesar más de 100 KB"
                )
                return None
            
            return func(*args, **kwargs)
        return _wrapper
    return _decorator


def validate_file(textfile_param_name: str) -> Callable[[Callable[P, T]], Callable[P, T | None]]:
    """
    Decorador que valida un archivo recibido como argumento
    en la función decorada.

    El decorador verifica que el parámetro especificado:
    - Existe dentro de los argumentos de la función.
    - Sea de tipo `str`.
    - Corresponda a un archivo existente.

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
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET} en argumentos: {e}")
                return None
            
            filename = bound.arguments.get(textfile_param_name)
            if filename is None:
                print(
                    f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                    f": No se encontró el parámetro '{textfile_param_name}'"
                )
                return None
            
            if not isinstance(filename, str):
                print(
                    f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                    f": El parámetro '{textfile_param_name}' debe ser un string"
                )
                return None
            
            if not _file_exists(filename):
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: El archivo '{filename}' no existe")
                return None
            
            return func(*args, **kwargs)
        return _wrapper
    return _decorator


def validate_key(
        condition_func: Callable[[K], bool],
        param_name: str = "key",
    ) -> Callable[[Callable[P, T]], Callable[P, T | None]]:
    def _decorator(func: Callable[P, T]) -> Callable[P, T | None]:
        @wraps(func)
        def _wrapper(*args: P.args, **kwargs: P.kwargs) -> T | None:
            try:
                bound = inspect.signature(func).bind(*args, **kwargs)
                bound.apply_defaults()
            except TypeError as e:
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET} en argumentos: {e}")
                return None
            
            key = bound.arguments.get(param_name)
            if key is None:
                print(
                    f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                    f": No se encontró el parámetro '{param_name}'"
                )
                return None

            if not condition_func(key):
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: La llave no es valida")
                return
            
            return func(*args, **kwargs)
        return _wrapper
    return _decorator
