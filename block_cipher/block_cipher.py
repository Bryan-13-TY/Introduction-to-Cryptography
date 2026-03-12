import numpy as np
import base64
import msvcrt
import os
from pathlib import Path

from Crypto.Cipher import DES
from Crypto.Util.Padding import pad

BASE_DIR = Path(__file__).parent

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(BASE_DIR / filename).exists()


def base64_to_binary(base64_str: str) -> str:
    byte_data = base64.b64decode(base64_str)
    binary_str = "".join(f"{byte:08b}" for byte in byte_data)
    return binary_str


def binary_to_base64(binary_str: str) -> str:
    byte_data = int(binary_str, 2).to_bytes(len(binary_str) // 8, byteorder="big")
    base64_str = base64.b64encode(byte_data).decode()
    return base64_str


def key_random_generator() -> None:
    num = np.random.randint(0, 128)
    binary_key = format(num, "08b")
    base64_key = binary_to_base64(binary_key)
    print(f"Tu llave K es la siguiente: {base64_key}")


def main() -> None:
    while True:
        _clean_console()
        print("""
/*-------------------.
| PERMUTATION CIPHER |
`-------------------*/
              
>> Elija una de las opciones
              
1.- Generar una llave aleatoria
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                key_random_generator()
                _wait_key()
            case "2":
                _wait_key()
            case "3":
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()