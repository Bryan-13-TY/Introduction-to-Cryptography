"""Cifrado usando AES."""

import base64
from pathlib import Path

from Crypto.Random import get_random_bytes
from Crypto.Cipher import AES

from config import BASE_DIR
from decorators import (
    validate_file,
    validate_key,
)
from utils import (
    clean_console,
    wait_key,
    error,
    success,
    yellow,
)

__all__ = ["aes_cipher_2_menu"]

def _is_valid_key(key_filename: str) -> bool:
    with open(BASE_DIR / key_filename, "r", encoding="utf-8") as f:
        data = f.read()
    
    try:
        base64.b64decode(data, validate=True)
        return True
    except Exception:
        return False


def _random_key_generator(key_size: int, key_file: str) -> None:
    if key_size not in (16, 24, 32):
        print(
            f"\n{yellow('>>')} "
            f"{error('ERROR')}: AES solo acepta llaves de 16, 24 o 32 bytes"
        )
        return

    key = get_random_bytes(key_size)
    key_filename = f"{key_file}_{key_size}.key"

    with open(BASE_DIR / key_filename, "w", encoding="utf-8") as f:
        f.write(base64.b64encode(key).decode())

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Llave guardada correctamente y guardado como {key_filename}')}"
    )


def _rebuild_key(key_filename: str) -> bytes:
    with open(BASE_DIR / key_filename, "r", encoding="utf-8") as f:
        data = f.read()

    return base64.b64decode(data)


@validate_file("key_filename")
@validate_key(_is_valid_key, "key_filename")
@validate_file("plaintext_filename")
def _encrypt_file(
    key_filename: str,
    plaintext_filename: str,
    ciphertext_filename: str
) -> None:
    with open(BASE_DIR / plaintext_filename, "rb") as f:
        data = f.read()

    key = _rebuild_key(key_filename)

    cipher = AES.new(key, AES.MODE_CTR)
    ciphertext = cipher.encrypt(data)

    suffix = Path(plaintext_filename).suffix
    nonce_b64 = base64.b64encode(cipher.nonce).decode()
    ciphertext_b64 = base64.b64encode(ciphertext).decode()

    cipher_filename = f"{ciphertext_filename}{suffix}.txt"


    with open(BASE_DIR / cipher_filename, "w") as f:
        f.write(nonce_b64 + "\n")
        f.write(ciphertext_b64)

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Archivo cifrado correctamente y guardado como {cipher_filename}')}"
    )


@validate_file("key_filename")
@validate_key(_is_valid_key, "key_filename")
@validate_file("ciphertext_filename")
def _decryp_file(
    key_filename: str,
    ciphertext_filename: str,
    recovered_filename: str,
) -> None:
    with open(BASE_DIR / ciphertext_filename, "r") as f:
        nonce_b64 = f.readline().strip()
        ciphertext_b64 = f.readline().strip()

    key = _rebuild_key(key_filename)

    # Decodificamos base64
    nonce = base64.b64decode(nonce_b64)
    ciphertext = base64.b64decode(ciphertext_b64)
    
    decipher = AES.new(key, AES.MODE_CTR, nonce=nonce)    
    recovered_plaintext = decipher.decrypt(ciphertext)
    cipher_no_suffix = ciphertext_filename.removesuffix(".txt")
    suffix = Path(cipher_no_suffix).suffix
    output_file = f"{recovered_filename}{suffix}"

    with open(BASE_DIR / output_file, "wb") as f:
        f.write(recovered_plaintext)

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Archivo recuperado correctamente y guardado como {output_file}')}"
    )
    

def aes_cipher_2_menu() -> None:
    while True:
        clean_console()
        print(f"""
/*--------------.
| AES CIPHER V2 |
`--------------*/
              
{yellow('>>')} Elija una de las opciones

1.- Crear una llave para AES
2.- Cifrar un archivo
3.- Descifrar un archivo
4.- Salir
""")
        option = input("Opción: ")
        match option:
            case "1":
                try:
                    key_size = int(input("\nEscribe el tamaño de la llave (16, 24 o 32 bytes): "))
                except Exception:
                    print(
                        f"\n{yellow('>>')} {error('ERROR')}"
                        ": Debe ser un número"
                    )
                    wait_key()
                    continue

                key_file = input(
                    "\nEscribe el nombre del archivo donde se almacenará la 'llave' (solo nombre): "
                )
                _random_key_generator(key_size, key_file)
                wait_key()
            case "2":
                key_filename = input("\nEscribe el nombre del archivo con la llave: ")
                infile = input("Escribe el nombre del archivo a cifrar: ")
                outfile = input("Escribe el nombre del archivo cifrado (solo nombre): ")
                _encrypt_file(key_filename, infile, outfile)
                wait_key()
            case "3":
                key_filename = input("\nEscribe el nombre del archivo con la llave: ")
                infile = input("Escribe el nombre del archivo cifrado: ")
                outfile = input("Escribe el nombre del archivo recuperado (solo nombre): ")
                _decryp_file(key_filename, infile, outfile)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{yellow('>>')} {error('ERROR')}: Opción no válida")
                wait_key()


def main() -> None:
    aes_cipher_2_menu()

if __name__ == "__main__":
    main()
