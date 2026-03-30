import numpy as np
import base64
import msvcrt
import os
from pathlib import Path

from Crypto.Cipher import DES
from Crypto.Util.Padding import pad, unpad

BASE_DIR = Path(__file__).parent

def _clean_console() -> None:
    os.system("cls" if os.name == "nt" else "clear")

def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()

def _file_exists(filename: str) -> bool:
   return Path(BASE_DIR / filename).exists()

def key_random_generator() -> str:
    """Genera una llave DES de 8 bytes en base64"""
    llave_bytes = np.random.bytes(8)
    llave_base64 = base64.b64encode(llave_bytes).decode()
    print(f"Llave DES generada (base64): {llave_base64}")
    print(f"Bytes: {' '.join(f'{b:02X}' for b in llave_bytes)}")
    return llave_base64

def encriptar_archivo_txt(plaintext_filename, llave_base64, ciphertext_filename=None):
    try:
        # Decodificar llave
        llave_bytes = base64.b64decode(llave_base64)
        if len(llave_bytes) != 8:
            print(f"Error: La llave debe tener 8 bytes. Tiene: {len(llave_bytes)}")
            return None

        # Construir ruta completa como en Hill Cipher
        ruta_completa_entrada = BASE_DIR / plaintext_filename
        
        # Verificar archivo de entrada
        if not ruta_completa_entrada.exists():
            print(f">> El archivo '{plaintext_filename}' no existe en {BASE_DIR}")
            return None

        # Leer archivo .txt (igual que Hill Cipher)
        with open(ruta_completa_entrada, 'r', encoding='utf-8') as f:
            texto_plano = f.read()

        datos_bytes = texto_plano.encode('utf-8')

        # Generar IV
        iv = os.urandom(8)

        # Encriptar
        cipher = DES.new(llave_bytes, DES.MODE_CBC, iv)
        datos_con_padding = pad(datos_bytes, DES.block_size)
        texto_cifrado = cipher.encrypt(datos_con_padding)

        # Determinar nombre de salida (igual que Hill Cipher)
        if ciphertext_filename is None:
            # Usar el mismo nombre pero con extensión .enc
            ciphertext_filename = str(Path(plaintext_filename).stem) + '.enc'
        
        # Construir ruta completa de salida (igual que Hill Cipher)
        ruta_completa_salida = BASE_DIR / ciphertext_filename

        # Guardar resultado
        with open(ruta_completa_salida, 'wb') as f:
            f.write(iv)
            f.write(texto_cifrado)
            
        #print(f"Ruta completa: {ruta_completa_salida}")
        #print(f"IV usado: {iv.hex()}")
        print(f"Tamaño cifrado: {len(texto_cifrado)} bytes")

        return str(ruta_completa_salida)

    except Exception as e:
        print(f"Error durante la encriptacion: {e}")
        return None

def descifrar_archivo(ciphertext_filename, llave_base64, plaintext_filename=None):
    try:
        # Decodificar llave
        llave_bytes = base64.b64decode(llave_base64)
        if len(llave_bytes) != 8:
            print(f"Error: La llave debe tener 8 bytes. Tiene: {len(llave_bytes)}")
            return None

        # Construir ruta completa de entrada (igual que Hill Cipher)
        ruta_completa_entrada = BASE_DIR / ciphertext_filename

        # Verificar archivo cifrado
        if not ruta_completa_entrada.exists():
            print(f">> El archivo '{ciphertext_filename}' no existe en {BASE_DIR}")
            return None

        # Leer archivo cifrado
        with open(ruta_completa_entrada, 'rb') as f:
            iv = f.read(8)
            texto_cifrado = f.read()

        # Descifrar
        cipher = DES.new(llave_bytes, DES.MODE_CBC, iv)
        datos_descifrados = cipher.decrypt(texto_cifrado)

        # Quitar padding
        try:
            datos_sin_padding = unpad(datos_descifrados, DES.block_size)
        except ValueError as e:
            print(f"Error de padding: {e}")
            print("Posiblemente la llave es incorrecta o el archivo esta corrupto")
            return None

        # Convertir a texto
        texto_descifrado = datos_sin_padding.decode('utf-8')

        # Determinar nombre de salida (igual que Hill Cipher)
        if plaintext_filename is None:
            if Path(ciphertext_filename).suffix == '.enc':
                nombre_base = Path(ciphertext_filename).stem
                plaintext_filename = f"{nombre_base}_descifrado.txt"
            else:
                plaintext_filename = str(Path(ciphertext_filename).stem) + '_descifrado.txt'
        
        # Construir ruta completa de salida
        ruta_completa_salida = BASE_DIR / plaintext_filename

        # Guardar archivo .txt
        with open(ruta_completa_salida, 'w', encoding='utf-8') as f:
            f.write(texto_descifrado)

        #print(f"Archivo descifrado guardado en: {plaintext_filename}")
        #print(f"Ruta completa: {ruta_completa_salida}")
        if len(texto_descifrado) > 100:
            print("Archvo descifrado guardado.")
            pass
        else:
            print(f"Contenido: {texto_descifrado}")

        return str(ruta_completa_salida)

    except Exception as e:
        print(f"Error durante el descifrado: {e}")
        return None
    
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
        option = input("Opcion: ")

        match option:
            case "1":
                key_random_generator()
                _wait_key()

            case "2":
                key = input("Ingresa tu llave K: ")
                filename = input("Ingresa el nombre de tu archivo: ")

                if not _file_exists(filename):
                    print("El archivo no existe. Verifica la ruta.")
                    _wait_key()
                    continue

                filename_encrypt = input("Ingresa el nombre con el que se guardara tu archivo cifrado (Enter para nombre automatico): ")
                if not filename_encrypt:
                    filename_encrypt = None

                encriptar_archivo_txt(filename, key, filename_encrypt)
                _wait_key()

            case "3":
                key = input("Ingresa tu llave K: ")
                filename = input("Ingresa el nombre de tu archivo cifrado: ")

                if not _file_exists(filename):
                    print("El archivo cifrado no existe. Verifica la ruta.")
                    _wait_key()
                    continue

                filename_decrypt = input("Ingresa el nombre con el que se guardara tu archivo descifrado (Enter para nombre automatico): ")
                if not filename_decrypt:
                    filename_decrypt = None

                descifrar_archivo(filename, key, filename_decrypt)
                _wait_key()

            case "4":
                print("¡Hasta luego!")
                break

            case _:
                print("Opcion no valida")
                _wait_key()

if __name__ == "__main__":
    main()
