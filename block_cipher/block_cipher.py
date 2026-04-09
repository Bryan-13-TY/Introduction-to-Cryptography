"""Cifrado usando DES."""

import base64

from Crypto.Random import get_random_bytes
from Crypto.Cipher import DES
from Crypto.Util.Padding import (
    pad,
    unpad,
)

from constants import (
    BASE_DIR_CRYPTO,
    RED,
    GREEN,
    YELLOW,
    RESET,
)
from decorators import (
    validate_key,
    validate_file_DES,
)
from utils import (
    clean_console,
    wait_key,
)

__all__ = ["block_cipher_menu"]

def _is_valid_key(key: bytes) -> bool:
    """
    Verifica que la llave en base 64 sea de 8 bytes.

    :param key: Llave en base 64.
    :type key: bytes
    :return: True si es de 8 bytes, False en caso contrario.
    :rtype: bool
    """
    return len(key) == 8


def _base64_to_binary(base64_str: str) -> str:
    decoded = base64.b64decode(base64_str)
    binary = "".join(format(byte, "08b") for byte in decoded)
    return binary


def _binary_to_base64(binary_str: str) -> str:
    data = int(binary_str, 2).to_bytes((len(binary_str) + 7) // 8, "big")
    encode = base64.b64encode(data)
    return encode.decode()


def _random_key_generator() -> bytes:
    """
    Genera una llave en bse 64 de 8 bytes.
    
    :return: Llave en base 64 de 8 bytes.
    :rtype: bytes
    """
    key = get_random_bytes(8)
    key_base64 = base64.b64encode(key).decode()
    print(f"\n{YELLOW}>>{RESET} Tu llave DES (Base64): {key_base64}")
    return key


@validate_key(_is_valid_key)
@validate_file_DES("plaintext_file")
def _encrypt_file(key: bytes, plaintext_file: str, ciphertext_file: str) -> None:
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
    with open(BASE_DIR_CRYPTO / plaintext_file, "rb") as f:
        data = f.read()

    iv = get_random_bytes(8)  # Bloque de 8 bytes para DES
    cipher = DES.new(key, DES.MODE_CBC, iv=iv)

    ciphertext = cipher.encrypt(pad(data, 8))

    with open(BASE_DIR_CRYPTO / ciphertext_file, "wb") as f:
        f.write(base64.b64encode(iv + ciphertext))

    print(
        f"\n{YELLOW}>>{RESET} {GREEN}"
        f"Archivo cifrado correctamente y guardado como '{ciphertext_file}'{RESET}"
    )


@validate_key(_is_valid_key)
@validate_file_DES("ciphertext_file")
def _decrypt_file(key: bytes, ciphertext_file: str, output_file: str) -> None:
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
    with open(BASE_DIR_CRYPTO / ciphertext_file, "rb") as f:
        data = base64.b64decode(f.read())

    iv = data[:8]
    ciphertext = data[8:]

    cipher = DES.new(key, DES.MODE_CBC, iv=iv)
    plaintext = unpad(cipher.decrypt(ciphertext), 8)

    with open(BASE_DIR_CRYPTO / output_file, "wb") as f:
        f.write(plaintext)
        
    print(
        f"\n{YELLOW}>>{RESET} {GREEN}"
        f"Archivo descifrado correctamente y guardado como '{output_file}'{RESET}"
    )


def block_cipher_menu() -> None:
    while True:
        clean_console()
        print(f"""
/*-------------.
| BLOCK CIPHER |
`-------------*/
              
{YELLOW}>>{RESET} Elija una de las opciones
              
1.- Crear una llave para DES 
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                _random_key_generator()
                wait_key()
            case "2":
                key_base64 = input("\nEscribe la llave en base 64: ")
                try:
                    key = base64.b64decode(key_base64)
                except Exception:
                    print(
                        f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                        ": La llave en base64 no es válida"
                    )
                    wait_key()
                    continue

                infile = input("Escribe el nombre del archivo con el 'plaintext': ")
                outfile = input(
                    "Escribe el nombre del archivo donde se almacenará el 'ciphertext': "
                )
                _encrypt_file(key, infile, outfile)
                wait_key()
            case "3":
                key_base64 = input("\nEscribe la llave en base 64: ")
                try:
                    key = base64.b64decode(key_base64)
                except Exception:
                    print(
                        f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}"
                        ": La llave en base64 no es válida"
                    )
                    wait_key()
                    continue

                infile = input("Escribe el nombre del archivo con el 'ciphertext': ")                
                outfile = input(
                    "Escribe el nombre del archivo donde se almacenará el 'plaintext': "
                )
                _decrypt_file(key, infile, outfile)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Opción no válida")
                wait_key()


def main() -> None:
    block_cipher_menu()

if __name__ == "__main__":
    main()
