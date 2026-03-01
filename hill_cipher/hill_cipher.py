import numpy as np
from pathlib import Path
import msvcrt
import os

__all__ = [
    "key_generation_hill",
    "encrypt_hill",
    "decypt_hill"
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
<<<<<<< Updated upstream
=======

    Examples
    --------
    >>> _get_num('A')
    33
>>>>>>> Stashed changes
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


def _gcd(a: int, b: int) -> int:
    """
    Devuelve el máximo común divisor de dos divisores `a` y `b`, usando
    el algoritmo extendido de Euclides.

    :param a: Primer divisor.
    :type a: int
    :param b: Segundo divisor:
    :type b: int
    :return: Máximo común divisor. 
    :rtype: int
    """
    a, b = abs(a), abs(b)
    while b != 0:
        a, b = b, a % b
    return a


def key_is_valid(key: np.ndarray) -> bool:
    determinant = _calculate_determinant(key)
    return _gcd(determinant, PRINTABLE_ASCII_LENGHT) == 1


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
    matrix = np.random.randint(low=0, high=PRINTABLE_ASCII_LENGHT, size=(2,2))
    return matrix


def _calculate_determinant(matrix: np.ndarray) -> int:
    a, b = matrix[0, 0], matrix[0, 1]
    c, d = matrix[1, 0], matrix[1, 1]
    determinant = int(a * d - b * c)
    return determinant


def _calculate_adjuntate_matrix(matrix: np.ndarray) -> np.ndarray:
    a, b = matrix[0, 0], matrix[0, 1]
    c, d = matrix[1, 0], matrix[1, 1]
    
    adjuntate_matrix = np.array([[d, -b], [-c, a]])
    return adjuntate_matrix 


def key_generation_hill() -> np.ndarray:
    while True:
        key = _generate_random_matrix()
        determinant = _calculate_determinant(key)
        if _gcd(determinant, PRINTABLE_ASCII_LENGHT) == 1:
            print(f"\nTu llave es K =\n{key}")
            return key


def _calculate_inverse_key(key: np.ndarray) -> np.ndarray | None:
    key_determinant = _calculate_determinant(key) % PRINTABLE_ASCII_LENGHT
    inverse_key_determinant = _multiplicative_inverse(PRINTABLE_ASCII_LENGHT, key_determinant) 

    if inverse_key_determinant is None:
        return None
    
    adjuntate_key = _calculate_adjuntate_matrix(key)
    inverse_key = (inverse_key_determinant * adjuntate_key) % PRINTABLE_ASCII_LENGHT
    return inverse_key


def convert_key_format(key: str) -> np.ndarray:
    key_list = key.split(", ")
    key_matrix = [
        [int(key_list[0]), int(key_list[1])],
        [int(key_list[2]), int(key_list[3])]
    ]
    valid_format_key = np.array(key_matrix)
    return valid_format_key


def encrypt_hill(
        key: np.ndarray,
        plaintext_filename: str,
        ciphertext_filename: str
    ) -> None:
    try:
        with open(BASE_DIR / plaintext_filename, "r", encoding="utf-8") as f:
            plaintext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'plaintext' no existe")
        return
    
    # Eliminamos los caracteres que no pertenecen al alfabeto
    text = [c for c in plaintext if c not in ("\n", "\t") and c not in VOCALES_ACENTUADAS]
    
    # Aseguramos longitud para dividir en bloques de dos
    if len(text) % 2 != 0:
        text.append("Ñ")

    ciphertext = ""
    i = 0
    while i < len(text):
        # Toma el texto en bloques de 2
        block = text[i:i + 2]
        # Convierte caracteres a números
        vector = np.array([_get_num(block[0]), _get_num(block[1])])
        # Multiplica el vector por la llave y se aplica el módulo
        cipher = np.dot(key, vector) % PRINTABLE_ASCII_LENGHT
        # Convierte a caracteres cifrados
        ciphertext += _get_char(cipher[0]) + _get_char(cipher[1])
        i += 2

    # Agregar los caracteres especiales originales
    for j, c in enumerate(plaintext):
        if c in ("\n", "\t") or c in VOCALES_ACENTUADAS:
            ciphertext = ciphertext[:j] + c + ciphertext[j:]

    try:
        with open(BASE_DIR / ciphertext_filename, "w", encoding="utf-8") as f:
            f.write(ciphertext)
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    

def decypt_hill(
        key: np.ndarray,
        ciphertext_filename: str
    ) -> None:
    try:
        with open(BASE_DIR / ciphertext_filename, "r", encoding="utf-8") as f:
            ciphertext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    
    # Eliminamos 
    text = [m for m in ciphertext if m not in ("\n", "\t") and m not in VOCALES_ACENTUADAS]
    # Aseguramos longitud para dividir en bloques de dos
    if len(text) % 2 != 0:
        text.append("Ñ")

    plaintext = ""
    inverse_key = _calculate_inverse_key(key)
    i = 0
    while i < len(text):
        # Toma el texto en bloques de 2
        block = text[i:i + 2]
        # Convierte caracteres a números
        vector = np.array([_get_num(block[0]), _get_num(block[1])])
        # Multiplica el vector por la lleva inversa y se aplica el módulo
        cipher = np.dot(inverse_key, vector) % PRINTABLE_ASCII_LENGHT
        # Convierte a caracteres cifrados
        plaintext += _get_char(cipher[0]) + _get_char(cipher[1])
        i += 2
    
    # Agregar los caracteres especiales originales
    for j, m in enumerate(ciphertext):
        if m in ("\n", "\t") or m in VOCALES_ACENTUADAS:
            plaintext = plaintext[:j] + m + ciphertext[j:]

    plaintext_lenght = len(plaintext)
        
    print(f">> Texto original recuperado:\n\n{plaintext[:plaintext_lenght - 1]}")


def main() -> None:
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
                key = key_generation_hill()
                print(f"\nY su inversa K^-1 =\n{_calculate_inverse_key(key)}")
                _wait_key()
            case "2":
                key = input("\nIngresa una llave válida (ej: [81, 63, 66, 85]): ").strip()
                key_valid_format = convert_key_format(key)
                if not key_is_valid(key_valid_format):
                    print(">> La llave no es válida")
                    return
                
                plaintext_filename = input("Escribe el nombre del archivo con el 'plaintext': ")
                if not _file_exists(plaintext_filename):
                    print(">> El archivo con el 'plaintext' no existe")
                    return
                
                ciphertext_filename = input(
                    "Escribe el nombre del archivo donde quieres guardar el ciphertext: "
                )
                encrypt_hill(key_valid_format, plaintext_filename, ciphertext_filename)
                _wait_key()
                pass
            case "3":
                key = input("\nIngresa una llave válida (ej: [81, 63, 66, 85]): ").strip()
                key_valid_format = convert_key_format(key)
                if not key_is_valid(key_valid_format):
                    print(">> La llave no es válida")
                    return

                ciphertext_filename = input("Escribe el nombre del archivo con el 'ciphertext': ")
                if not _file_exists(ciphertext_filename):
                    print(">> El archivo con el 'ciphertext' no existe")
                    return
                
                decypt_hill(key_valid_format, ciphertext_filename)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
