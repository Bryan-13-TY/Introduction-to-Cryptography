import numpy as np
from pathlib import Path
import msvcrt
import os

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def permutation_generator(permutation_size: int) -> np.ndarray:
    random_permutation = np.random.permutation(np.arange(1, permutation_size + 1))
    return random_permutation


def inverse_permutation_generator(random_permutation: np.ndarray) -> np.ndarray:
    inverse_permutation = [
        random_permutation.tolist().index(i) + 1
        for i in range(1, len(random_permutation) + 1)
    ]
    return np.array(inverse_permutation)


def permutation_random_generator(permutation_size: int) -> np.ndarray:
    pass


def save_permutation() -> None:
    pass


def main() -> None:
    while True:
        _clean_console()
        print("""
/*-------------------.
| PERMUTATION CIPHER |
`-------------------*/
              
>> Elija una de las opciones
              
1.- Crear una permutación π de tamaño n
2.- Cifrar el texto plano
3.- Descifrar el texto cifrado
4.- Salir 
""")
        option = input("Opción: ")
        match option:
            case "1":
                permutation_size = input("\nIngresa el tamaño que tendrá la permutación: ")
                if not permutation_size.isdigit():
                    print(">> Debe ser un número")
                else:
                    permutation_random_generator(int(permutation_size))
               
                _wait_key()
            case "2":
                pass
            case "3":
                pass
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
