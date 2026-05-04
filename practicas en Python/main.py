from permutation_cipher import permutation_cipher_menu
from block_cipher import block_cipher_menu, file_generator_DES
from affin_cipher import affin_cipher_menu
from hill_cipher import hill_cipher_menu
from shift_cipher import shift_cipher_menu

from utils import (
    clean_console,
    wait_key,
)
from constants import (
    RED,
    YELLOW,
    RESET,
)

def main() -> None:
    while True:
        clean_console()
        print(f"""
/*----------------------------------------------------.
| INTRODUCTION TO CRYPTOGRAPHY: Algoritmos de cifrado |              
`----------------------------------------------------*/
              
{YELLOW}>>{RESET} Elija una de las opciones
              
1.- Cifrar usando 'Shift Cipher'
2.- Cifrar usando 'Affin Cipher'
3.- Cifrar usando 'Hill Cipher'
4.- Cifrar usando 'Permutation Cipher'
5.- Cifrar usando 'DES'
6.- Salir
""")
        option = input("Opción: ")
        match option:
            case "1":
                shift_cipher_menu()
            case "2":
                affin_cipher_menu()
            case "3":
                hill_cipher_menu()
            case "4":
                permutation_cipher_menu()
            case "5":
                block_cipher_menu()
            case "6":
                break
            case _:
                print(f"\n{YELLOW}>>{RESET}{RED} ERROR{RESET}: Opción no válida")
                wait_key()

if __name__ == "__main__":
    main()
