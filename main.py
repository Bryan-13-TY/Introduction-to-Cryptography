from src import (
    ENGLISH_ALPHABET,
    SPANISH_ALPHABET,
    GERMANY_ALPHABET,
)

from shift_cipher import (
    encrypt_shift,
    decrypt_shift,
)

from affin_cipher import (
    key_generator_affin,
    encrypt_affin,
    decrypt_affin
)

def main() -> None:
    plane_text = input("Escribe algo: ")

    try:
        key = int(input("Escribe la llave de cifrado: "))
    except ValueError:
        print(">> Deber ser un número entero")
        return
    
    print("""
Elije el alfabeto a usar
          
1.- Alfabeto inglés
2.- Alfabeto español
3.- Alfabeto alemán
""")
    
    option = input("Opción: ")
    match option:
        case "1":
            alphabet = ENGLISH_ALPHABET
        case "2":
            alphabet = SPANISH_ALPHABET
        case '3':
            alphabet = GERMANY_ALPHABET
        case _:
            print(">> Opción no valida")
            return
    
    encrypt_text = encrypt_shift(key, plane_text, alphabet)
    decrypt_text = decrypt_shift(key, encrypt_text, alphabet)

    print(f"{encrypt_text=}\n{decrypt_text=}")

if __name__ == "__main__":
    main()
