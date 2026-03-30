import base64
import msvcrt
import os
from pathlib import Path

from Crypto.Random import get_random_bytes
from Crypto.Cipher import DES
from Crypto.Util.Padding import pad
from Crypto.Util.Padding import unpad

BASE_DIR = Path(__file__).parent

def _clean_console() -> None:
    os.system("cls" if os.name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(BASE_DIR / filename).exists()


def _is_valid_key(key: bytes) -> bool:
    return len(key) == 8


def _is_valid_file_size(filename: str) -> bool:
    size = os.path.getsize(BASE_DIR / filename)
    return size > 100 * 1024


def base64_to_binary(base64_str: str) -> str:
    decoded = base64.b64decode(base64_str)
    binary = "".join(format(byte, "08b") for byte in decoded)
    return binary


def binary_to_base64(binary_str: str) -> str:
    data = int(binary_str, 2).to_bytes((len(binary_str) + 7) // 8, "big")
    encode = base64.b64encode(data)
    return encode.decode()


def random_key_generator() -> bytes:
    key = get_random_bytes(8)
    key_base64 = base64.b64encode(key).decode()
    print(f"\nLlave DES (Base64): {key_base64}")
    return key


def encrypt_file(key: bytes, plaintext_file: str, ciphertext_file) -> None:
    with open(BASE_DIR / plaintext_file, "rb") as f:
        data = f.read()

    iv = get_random_bytes(8)  # Bloque de 8 bytes para DES
    cipher = DES.new(key, DES.MODE_CBC, iv=iv)

    ciphertext = cipher.encrypt(pad(data, 8))

    with open(BASE_DIR / ciphertext_file, "wb") as f:
        f.write(base64.b64encode(iv + ciphertext))

    print("Archivo encriptado")


def decrypt_file(key: bytes, ciphertext_file: str, output_file: str) -> None:
    with open(BASE_DIR / ciphertext_file, "rb") as f:
        data = base64.b64decode(f.read())

    iv = data[:8]
    ciphertext = data[8:]

    cipher = DES.new(key, DES.MODE_CBC, iv=iv)
    plaintext = unpad(cipher.decrypt(ciphertext), 8)

    with open(BASE_DIR / output_file, "wb") as f:
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

                if not _is_valid_key(key):
                    print(">> La llave DES debe tener exactamente 8 bytes (64 bits)")
                    _wait_key()
                    continue

                infile = input("Escribe el nombre del archivo con el 'plaintext': ")
                if not _file_exists(infile):
                    print(">> El archivo con el 'plaintext' no existe")
                    _wait_key()
                    continue
                if not _is_valid_file_size(infile):
                    print(">> El archivo debe ser mayor a 100 KB")
                    _wait_key()
                    continue

                outfile = input("Escribe el nombre del archivo donde se almacenará el 'ciphertext': ")

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

                if not _is_valid_key(key):
                    print(">> La llave DES debe tener exactamente 8 bytes (64 bits)")
                    _wait_key()
                    continue

                infile = input("Escribe el nombre del archivo con el 'ciphertext': ")
                if not _file_exists(infile):
                    print(">> El archivo con el 'ciphertext' no existe")
                    _wait_key()
                    continue
                if not _is_valid_file_size(infile):
                    print(">> El archivo debe ser mayor a 100 KB")
                    _wait_key()
                    continue
                
                outfile = input("Escribe el nombre del archivo donde se almacenará el 'plaintext': ")
                
                decrypt_file(key, infile, outfile)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

def main2() -> None:
    print("Probando la conversión de binario a base64")
    print(f"\n01000001 a base64: {binary_to_base64("01000001")}")
    print(f"0100000101000010 a base64: {binary_to_base64("0100000101000010")}")
    print(f"010000010100001001000011 a base64: {binary_to_base64("010000010100001001000011")}")
    print(f"0100100001101111 a base64: {binary_to_base64("0100100001101111")}")
    print(f"01001000011011110110110001100001 a base64: {binary_to_base64("01001000011011110110110001100001")}")
    print("\nProbando la conversión de base64 a binario")
    print(f"\nQQ== a binario: {base64_to_binary("QQ==")}")
    print(f"QUI= a binario: {base64_to_binary("QUI=")}")
    print(f"QUJD a binario: {base64_to_binary("QUJD")}")
    print(f"SG8= a binario: {base64_to_binary("SG8=")}")
    print(f"SG9sYQ== a binario: {base64_to_binary("SG9sYQ==")}")

if __name__ == "__main__":
    main()
