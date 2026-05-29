"""Cifrado usando AES."""

import base64

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

__all__ = ["aes_cipher_menu"]

def _is_valid_key(key_filename: str) -> bool:
    with open(BASE_DIR / key_filename, "r", encoding="utf-8") as f:
        data = f.read()
    try:
        base64.b64decode(data, validate=True)
        return True
    except Exception:
        return False


def _random_key_generator(key_size: int, key_file: str) -> None:
    key = get_random_bytes(key_size)

    with open(BASE_DIR / key_file, "w", encoding="utf-8") as f:
        f.write(base64.b64encode(key).decode())

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Llave guardada correctamente y guardado como {key_file}')}"
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

    with open(BASE_DIR / f"{ciphertext_filename}.enc", "wb") as f:
        # Guardamos el tamaño del nonce
        f.write(len(cipher.nonce).to_bytes(1, 'big')) 
        # Guardamos el nonce
        f.write(cipher.nonce)
        f.write(ciphertext)

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Archivo cifrado correctamente y guardado como {ciphertext_filename}')}"
    )


@validate_file("key_filename")
@validate_key(_is_valid_key, "key_filename")
@validate_file("ciphertext_filename")
def _decryp_file(
    key_filename: str,
    ciphertext_filename: str,
) -> None:
    with open(BASE_DIR / ciphertext_filename, "rb") as f:
        # Recuperamos el tamaño del once
        nonce_size = int.from_bytes(f.read(1), 'big')
        # Recuperamos el nonce
        nonce = f.read(nonce_size)
        ciphertext = f.read()

    key = _rebuild_key(key_filename)
    decipher = AES.new(key, AES.MODE_CTR, nonce=nonce)

    recovered_plaintext = decipher.decrypt(ciphertext)
    output_file = ciphertext_filename.removesuffix(".enc")

    with open(BASE_DIR / output_file, "wb") as f:
        f.write(recovered_plaintext)

    print(
        f"\n{yellow('>>')}"
        f"{success(f'Archivo recuperado correctamente y guardado como {output_file}')}"
    )
    

def aes_cipher_menu() -> None:
    while True:
        clean_console()
        print(f"""
/*-----------.
| AES CIPHER |
`-----------*/
              
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
                    key_size = int(input("\nEscribe el tamaño de la llave (bytes): "))
                except Exception:
                    print(
                        f"\n{yellow('>>')} {error('ERROR')}"
                        ": Debe ser un número"
                    )
                    wait_key()
                    continue

                key_file = input(
                    "\nEscribe el nombre del archivo donde se almacenará la 'llave': "
                )
                _random_key_generator(key_size, key_file)
                wait_key()
            case "2":
                key_filename = input("\nEscribe el nombre del archivo con la llave: ")
                infile = input("Escribe el nombre del archivo a cifrar: ")
                outfile = input("Escribe el nombre del archivo del archivo cifrado: ")
                _encrypt_file(key_filename, infile, outfile)
                wait_key()
            case "3":
                key_filename = input("\nEscribe el nombre del archivo con la llave: ")
                infile = input("Escribe el nombre del archivo cifrado: ")
                _decryp_file(key_filename, infile)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{yellow('>>')} {error('ERROR')}: Opción no válida")
                wait_key()


def main() -> None:
    aes_cipher_menu()

if __name__ == "__main__":
    main()
