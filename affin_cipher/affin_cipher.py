import numpy as np
from pathlib import Path
import os
import msvcrt
import ast

__all__ = [
    "key_generator_affin",
    "encrypt_affin",
    "decrypt_affin"
]

BASE_DIR = Path(__file__).parent
PRINTABLE_ASCII_LENGHT = 95
VOCALES_ACENTUADAS = "ÁÉÍÓÚáéíóú"

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(BASE_DIR / filename).exists()


def _get_num(character: str) -> int:
    """
    Devuelve el código Unicode normalizado a 0 - 94. Solo se usa para
    los caracteres imprimibles desde la 32 - 126.

    :param character: Carácter imprimible.
    :type character: str
    :return: Código Unicode correspondiente a un carácter.
    :rtype: int
    """
    unicode = ord(character) - 32
    return unicode


def _get_char(unicode: int) -> str:
    """
    Devuelve el cáracter que corresponde al código Unicode normalizado
    a 0 - 94.

    :param unicode: Unicode normalizado a 0 - 94.
    :type unicode: int
    :return: Cáracter correspondiente al código Unicode.
    :rtype: str
    """
    char = chr(unicode + 32)
    return char


def key_is_valid(key: tuple[int, int]) -> bool:
    coprimes = _coprime_numbers(PRINTABLE_ASCII_LENGHT)
    a, b = key

    return a in coprimes and 0 <= b <= PRINTABLE_ASCII_LENGHT - 1


def _gcd(a: int, b: int) -> int:
    """
    Devuelve el máximo común divisor de dos divisores `a` y `b`.

    :param a: Primer divisor.
    :type a: int
    :param b: Segundo divisor:
    :type b: int
    :return: Máximo común divisor o 1 si este no se encuentra. 
    :rtype: int
    """
    # Divisor común trivial
    res = 1

    while a != 1 and b != 1:
        factor_is_found = False

        for i in range(2, min(a, b) + 1):
            if a % i == 0 and b % i == 0:
                res *= i
                a //= i
                b //= i
                factor_is_found = True
                break

        if not factor_is_found:
            break
    return res


def _coprime_numbers(n: int) -> list[int] | None:
    """
    Devuelve una lista con los números que con coprimos con `n`.

    Un número es coprimo con `n` si el máximo común divisor (MCD)
    entre ambos es igual a 1.

    :param n: Número entero mayor o igual a 2.
    :type n: int
    :return: Lista de enteros coprimos con `n` en el rango [1, n),
             o None si `n` es menor que 2.
    :rtype: list[int] | None
    """
    if not n >= 2:
        return None
    
    coprimes = [i for i in range(1, n) if _gcd(i, n) == 1]
    return coprimes


def _multiplicative_inverse(n: int, a: int) -> int | None:
    """
    Devuelve el inverso multiplicativo `b` perteneciente al conjunto
    de los coprimos con `n`, tal que `a * b % n = 1`.

    :param n: Número entero mayor o igual a 2.
    :type n: int
    :param a: Description.
    :type a: int
    :return: Inverso multiplicativo o None si `n` es menor que 2 y si
             `a` no es comprimo con `n`.
    :rtype: int | None
    """
    if not n >= 2:
        return None
    
    coprimes = _coprime_numbers(n)
    if coprimes is None or a not in coprimes:
        return None
    
    b = None

    for coprime in coprimes:
        if not a * coprime % n == 1:
            continue

        b = coprime
        break

    return b


def key_generator_affin() -> None:
    """
    Imprime una llave válida para el cifrado afin y los caracteres
    imprimibles ASCII como alfabeto.
    """
    coprimes = _coprime_numbers(PRINTABLE_ASCII_LENGHT)
    if coprimes is None:
        return
    
    rng = np.random.default_rng()
    a = rng.choice(coprimes)
    b = rng.integers(0, PRINTABLE_ASCII_LENGHT)

    if _multiplicative_inverse(PRINTABLE_ASCII_LENGHT, a) is None:
        return None
    
    key = np.array([a, b])
    print(f"\nTu llave es K = {key}")


def encrypt_affin(
        key: tuple[int, int],
        plaintext_filename: str,
        ciphertext_filename: str
    ) -> None:
    """
    Cifra un texto desde el archivo `plaintext_filename` usando el
    algoritmo 'Cifrado afin' y lo guarda en el archivo
    `ciphertext_filename`.

    :param key: Llave válida.
    :type key: tuple[int, int]
    :param plaintext_filename: Nombre del archivo con el 'plaintext'.
    :type plaintext_filename: str
    :param ciphertext_filename: Nombre del archivo con el 'ciphertext'.
    :type ciphertext_filename: str
    """
    try:
        with open(BASE_DIR / plaintext_filename, "r", encoding="utf-8") as f:
            plaintext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'plaintext' no existe")
        return
    
    ciphertext = ""
    a, b = key
    
    for m in plaintext:
        if m in ("\n", "\t") or m in VOCALES_ACENTUADAS:
            # Solo concatenamos el carácter
            ciphertext += m
        else:
            c = (a * _get_num(m) + b) % PRINTABLE_ASCII_LENGHT
            ciphertext += _get_char(c)

    try:
        with open(BASE_DIR / ciphertext_filename, "w", encoding="utf-8") as f:
            f.write(ciphertext)
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return


def decrypt_affin(
        key: tuple[int, int],
        ciphertext_filename: str
    ) -> None:
    """
    Descifra un texto desde el `ciphertext_filename` usando el
    algoritmo 'Descifrado afin' y lo imprime, así como el
    inverso multiplicativo usado.

    :param key: Llave válida.
    :type key: tuple[int, int]
    :param ciphertext_filename: Nombre del archivo con el 'ciphertext'.
    :type ciphertext_filename: str
    """
    try:
        with open(BASE_DIR / ciphertext_filename, "r", encoding="utf-8") as f:
            ciphertext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    
    plaintext = ""
    a, b = key
    a_1 = _multiplicative_inverse(PRINTABLE_ASCII_LENGHT, a)
    
    for c in ciphertext:
        if c in ("\n", "\t") or c in VOCALES_ACENTUADAS:
            # Solo concatenamos el carácter
            plaintext += c
        else:
            m = ((_get_num(c) - b) * a_1) % PRINTABLE_ASCII_LENGHT
            plaintext += _get_char(m)

    print(f"\n>> El inverso multiplicativo usando fue: {a_1}")
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
                if not key_is_valid(key_tuple):
                    print(">> La llave no es válida")
                    return
                
                plaintext_filename = input("Escribe el nombre del archivo con el 'plaintext': ")
                if not _file_exists(plaintext_filename):
                    print(">> El archivo con el 'plaintext' no existe")
                    return
                
                ciphertext_filename = input(
                    "Escribe el nombre del archivo donde quieres guardar el ciphertext: "
                )
                encrypt_affin(key_tuple, plaintext_filename, ciphertext_filename)
                _wait_key()
            case "3":
                key = input("\nIngresa una llave válida (ej: (49, 82)): ").strip()
                key_tuple = ast.literal_eval(key)
                if not key_is_valid(key_tuple):
                    print(">> La llave no es válida")
                    return

                ciphertext_filename = input("Escribe el nombre del archivo con el 'ciphertext': ")
                if not _file_exists(ciphertext_filename):
                    print(">> El archivo con el 'ciphertext' no existe")
                    return
                
                decrypt_affin(key_tuple, ciphertext_filename)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
