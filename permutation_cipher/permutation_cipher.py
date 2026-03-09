import numpy as np
from pathlib import Path
import msvcrt
import os

BASE_DIR = Path(__file__).parent

def _clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()


def _file_exists(filename: str) -> bool:
    return Path(BASE_DIR / filename).exists()


def _convert_permutation_to_string(permutation: np.ndarray) -> str:
    str_permutation = ""
    for p in permutation:
        str_permutation += f"{p} "
    return str_permutation.strip()


def _convert_permutation_to_array(str_permutation: str) -> np.ndarray:
    recover_permutation = str_permutation.split()
    array_permutation = np.array([int(p) for p in recover_permutation])
    return array_permutation


def _recover_permutation_from_file(permutation_file: str) -> np.ndarray:
    with open(BASE_DIR / permutation_file, "r", encoding="utf-8") as f:
        permutation = f.read()

    array_permutation = _convert_permutation_to_array(permutation)
    return array_permutation


def _save_permutation_in_file(permutation: np.ndarray) -> None:
    str_permutation = _convert_permutation_to_string(permutation)
    try:
        with open(BASE_DIR / f"permutation_{len(permutation)}.txt", "w", encoding="utf-8") as f:
            f.write(str_permutation)
    except FileNotFoundError:
        print(f"El archivo con el 'permutation_{len(permutation)}.txt' no existe")
        return


def _permutation_generator(permutation_size: int) -> np.ndarray:
    random_permutation = np.random.permutation(np.arange(1, permutation_size + 1))
    return random_permutation


def _inverse_permutation_generator(random_permutation: np.ndarray) -> np.ndarray:
    inverse_permutation = [
        random_permutation.tolist().index(i) + 1
        for i in range(1, len(random_permutation) + 1)
    ]
    return np.array(inverse_permutation)


def permutation_random_generator(permutation_size: int) -> tuple[np.ndarray, np.ndarray]:
    permutation = _permutation_generator(permutation_size)
    inverse_permutation = _inverse_permutation_generator(permutation)
    _save_permutation_in_file(permutation)
    return permutation, inverse_permutation


def encrypt_permutation(plaintext: str, permutation_file: str) -> None:    
    permutation = _recover_permutation_from_file(permutation_file)
    block_size = len(permutation)
    
    # Padding con X, para asegurar la dividión en bloques de 'block_size'
    while len(plaintext) % len(permutation) != 0:
        plaintext += "X"
        
    # Dividir en bloques de 'block_size'
    plaintext_blocks: list[str] = []
    i = 0
    while i < len(plaintext):
        block = plaintext[i:i+block_size]
        plaintext_blocks.append(block)
        i += block_size
   
    # Cifrar bloque por bloque
    aux_plaintext_blocks: list[str] = []
    aux_block = ""
    for block in plaintext_blocks:
        for c in range(block_size):
            index = permutation[c]
            aux_block += block[index - 1]
        
        aux_plaintext_blocks.append(aux_block)
        aux_block = ""

    ciphertext = "".join(aux_plaintext_blocks)
    
    print("\nEl texto cifrado es el siguiente:\n\n", ciphertext)


def decrypt_permutation(ciphertext: str, permutation_file: str) -> None:
    permutation = _recover_permutation_from_file(permutation_file)
    inverse_permutation = _inverse_permutation_generator(permutation)
    block_size = len(permutation)

    # Dividir en bloques de 'block_size'
    ciphertext_blocks: list[str] = []
    i = 0
    while i < len(ciphertext):
        block = ciphertext[i:i+block_size]
        ciphertext_blocks.append(block)
        i += block_size

    # Descifrar bloque por bloque
    aux_ciphertex_blocks: list[str] = []
    aux_block = ""
    for block in ciphertext_blocks:
        for c in range(block_size):
            index = inverse_permutation[c]
            aux_block += block[index - 1]

        aux_ciphertex_blocks.append(aux_block)
        aux_block = ""

    plaintext = "".join(aux_ciphertex_blocks)

    print("\nEl texto original recuperado es el siguiente:\n\n", plaintext)


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
                    return
                
                (
                    permutation,
                    inverse_permutation
                ) = permutation_random_generator(int(permutation_size))

                print(f"\nLa permutación generada es π(x):\n\n", permutation)
                print(f"\nY su inversa π^-1(x):\n\n", inverse_permutation)               
                _wait_key()
            case "2":
                plaintext = input("\nIngresa el 'plaintext' M: ")
                file_permutation = input("Escribe el nombre del archivo con la permutación a usar: ")
                if not _file_exists(file_permutation):
                    print(">> El archivo con la 'permutación' no existe")
                    return
                
                encrypt_permutation(plaintext, file_permutation)
                _wait_key()
                pass
            case "3":
                ciphertext = input("\nIngresa el 'ciphertext' C: ")
                file_permutation = input("Escribe el nombre del archivo con la permutación a usar: ")
                if not _file_exists(file_permutation):
                    print(">> El archivo con la 'permutación' no existe")
                    return

                decrypt_permutation(ciphertext, file_permutation)
                _wait_key()
            case "4":
                break
            case _:
                print(">> Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()
