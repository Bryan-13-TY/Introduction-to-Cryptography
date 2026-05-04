from constants import (
    BASE_DIR_CRYPTO,
    RED,
    GREEN,
    YELLOW,
    RESET,
    SPANISH_ALPHABET,
)
from decorators import validate_file
from utils import (
    clean_console,
    wait_key,
)

__all__ = ["shift_cipher_menu"]

@validate_file("plaintext_file")
def _encrypt_shift(
        key: int,
        plaintext_file: str,
        ciphertext_file: str,
    ) -> None:
    with open(BASE_DIR_CRYPTO / plaintext_file, "r", encoding="utf-8") as f:
        plaintext = f.read()

    alphabet_lenght = len(SPANISH_ALPHABET)
    ciphertext = ""
    
    for m in plaintext:
        if m in SPANISH_ALPHABET:
            residue = (SPANISH_ALPHABET.find(m) + key) % alphabet_lenght
            ciphertext += SPANISH_ALPHABET[residue]
        else:
            ciphertext += m
    
    with open(BASE_DIR_CRYPTO / ciphertext_file, "w", encoding="utf-8") as f:
        f.write(ciphertext)

    print(
        f"\n{YELLOW}>>{RESET} {GREEN}"
        f"Texto cifrado correctamente y guardado en '{ciphertext_file}'{RESET}"
    )


@validate_file("ciphertext_file")
def _decrypt_shift(
        key: int,
        ciphertext_file: str,
    ) -> None:
    try:
        with open(BASE_DIR_CRYPTO / ciphertext_file, "r", encoding="utf-8") as f:
            ciphertext = f.read()
    except FileNotFoundError:
        print(">> El archivo con el 'ciphertext' no existe")
        return
    
    alphabet_lenght = len(SPANISH_ALPHABET)
    plaintext = ""

    for c in ciphertext:
        if c in SPANISH_ALPHABET:
            residue = (SPANISH_ALPHABET.find(c) - key) % alphabet_lenght
            plaintext += SPANISH_ALPHABET[residue]
        else:
            plaintext += c
    
    print(f"\n{YELLOW}>>{RESET} Texto original recuperado:\n\n{plaintext}")


def shift_cipher_menu() -> None:
    while True:
        clean_console()
        print(f"""
/*-------------.
| SHIFT CIPHER |
`-------------*/
              
{YELLOW}>>{RESET} Elija una de las opciones
              
1.- Cifrar el texto plano
2.- Descifrar el texto cifrado
3.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                try:
                    key = int(input("\nIngresa una llave: "))
                except ValueError:
                    print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Debe ser un número")
                    wait_key()
                    continue

                plaintext_filename = input("Escribe el nombre del archivo con el 'plaintext': ")
                ciphertext_filename = input("Escribe el nombre del archivo donde se almacenará el 'ciphertext': ")
                _encrypt_shift(key, plaintext_filename, ciphertext_filename)
                wait_key()
            case "2":
                try:
                    key = int(input("\nIngresa una llave: "))
                except ValueError:
                    print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET} Debe ser un número")
                    wait_key()
                    continue

                cipher_filename = input("Escribe el nombre del archivo con el 'ciphertext': ")
                _decrypt_shift(key, cipher_filename)
                wait_key()
            case "3":
                break
            case _:
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Opción no válida")
                wait_key()


def main() -> None:
    shift_cipher_menu()

if __name__ == "__main__":
    main()
