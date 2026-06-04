"""Cifrado usando AES."""

import base64
from pathlib import Path
import time
from tqdm import tqdm

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

CHUNK_SIZE = 1024 * 1024  # 1 MB

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
        f"{success(f'Llave generada correctamente y guardada como {key_filename}')}"
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
    ciphertext_filename: str,
) -> None:
    key = _rebuild_key(key_filename)
    suffix = Path(plaintext_filename).suffix
    input_file = BASE_DIR / plaintext_filename
    cipher_filename = f"{ciphertext_filename}{suffix}.enc"
    output_file = BASE_DIR / cipher_filename

    cipher = AES.new(key, AES.MODE_CTR)
    file_size = input_file.stat().st_size 

    start = time.perf_counter()

    with open(input_file, "rb") as fin, open(output_file, "wb") as fout:
        fout.write(len(cipher.nonce).to_bytes(1, 'big'))
        fout.write(cipher.nonce)

        with tqdm(
            total=file_size,
            unit="B",
            unit_scale=True,
            desc="Cifrando",
        ) as pbar:
            while chunk := fin.read(CHUNK_SIZE):
                fout.write(cipher.encrypt(chunk))
                pbar.update(len(chunk))

    elapsed = time.perf_counter() - start
    speed_mb = (file_size / (1024 * 1024)) / elapsed

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Archivo cifrado correctamente y guardado como {cipher_filename}')}"
    )
    print(f"{yellow('>>')} Tiempo de cifrado: {elapsed:.3f} segundos")
    print(f"{yellow('>>')} Velocidad promedio: {speed_mb:.2f} MB/s")


@validate_file("key_filename")
@validate_key(_is_valid_key, "key_filename")
@validate_file("ciphertext_filename")
def _decrypt_file(
    key_filename: str,
    ciphertext_filename: str,
) -> None:
    key = _rebuild_key(key_filename)
    input_file = BASE_DIR / ciphertext_filename
    recover_filename = ciphertext_filename.removesuffix(".enc")
    output_file = BASE_DIR / recover_filename

    start = time.perf_counter()
    file_size = input_file.stat().st_size

    with open(input_file, "rb") as fin, open(output_file, "wb") as fout:
        nonce_len = int.from_bytes(fin.read(1), 'big')
        nonce = fin.read(nonce_len)

        decipher = AES.new(key, AES.MODE_CTR, nonce=nonce)

        with tqdm(
            total=file_size,
            unit="B",
            unit_scale=True,
            desc="Descifrado",
        ) as pbar:
            while chunk := fin.read(CHUNK_SIZE):
                fout.write(decipher.decrypt(chunk))
                pbar.update(len(chunk))

    elapsed = time.perf_counter() - start
    speed_mb = (file_size / (1024 * 1024)) / elapsed

    print(
        f"\n{yellow('>>')} "
        f"{success(f'Archivo recuperado correctamente y guardado como {recover_filename}')}"
    )
    print(f"{yellow('>>')} Tiempo de descifrado: {elapsed:.3f} segundos")
    print(f"{yellow('>>')} Velocidad promedio: {speed_mb:.2f} MB/s")    


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
                    key_size = int(input("\nEscribe el tamaño de la llave (16, 24 o 32 bytes): "))
                except Exception:
                    print(
                        f"\n{yellow('>>')} {error('ERROR')}"
                        ": Debe ser un número"
                    )
                    wait_key()
                    continue

                key_file = input(
                    "Escribe el nombre del archivo donde se almacenará la 'llave' (solo nombre): "
                )
                _random_key_generator(key_size, key_file)
                wait_key()
            case "2":
                key_filename = input("\nEscribe el nombre del archivo con la llave: ")
                infile = input("Escribe el nombre del archivo a cifrar: ")
                outfile = input("Escribe el nombre del archivo cifrado (solo nombre): ")
                print()
                _encrypt_file(key_filename, infile, outfile)
                wait_key()
            case "3":
                key_filename = input("\nEscribe el nombre del archivo con la llave: ")
                infile = input("Escribe el nombre del archivo cifrado: ")
                print()
                _decrypt_file(key_filename, infile)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{yellow('>>')} {error('ERROR')}: Opción no válida")
                wait_key()


def main() -> None:
    pass

if __name__ == "__main__":
    main()
