import numpy as np
import base64
import msvcrt
import os
from pathlib import Path

from Crypto.Cipher import DES
from Crypto.Util.Padding import pad, unpad

# Usar el directorio actual de trabajo
DIRECTORIO_ACTUAL = Path.cwd()

def _clean_console() -> None:
    os.system("cls" if os.name == "nt" else "clear")

def _wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()

def _file_exists(filename: str) -> bool:
    # Busca el archivo en el directorio actual
    ruta_completa = DIRECTORIO_ACTUAL / filename
    existe = ruta_completa.exists()
    if not existe:
        print(f"Buscando en: {ruta_completa}")
    return existe

def _ruta_en_directorio_actual(filename: str) -> Path:
    # Devuelve la ruta completa en el directorio actual
    return DIRECTORIO_ACTUAL / filename

def key_random_generator() -> str:
    """Genera una llave DES de 8 bytes en base64"""
    llave_bytes = np.random.bytes(8)
    llave_base64 = base64.b64encode(llave_bytes).decode()
    print(f"Llave DES generada (base64): {llave_base64}")
    print(f"Bytes: {' '.join(f'{b:02X}' for b in llave_bytes)}")
    return llave_base64

def encriptar_archivo_txt(ruta_txt, llave_base64, ruta_salida=None):
    """Encripta un archivo .txt usando DES en el directorio actual"""
    try:
        # Decodificar llave
        llave_bytes = base64.b64decode(llave_base64)
        if len(llave_bytes) != 8:
            print(f"Error: La llave debe tener 8 bytes. Tiene: {len(llave_bytes)}")
            return None

        # Construir rutas en el directorio actual
        archivo_entrada = _ruta_en_directorio_actual(ruta_txt)
        
        # Verificar archivo de entrada
        if not archivo_entrada.exists():
            print(f"Error: El archivo {ruta_txt} no existe en {DIRECTORIO_ACTUAL}")
            return None

        # Leer archivo .txt
        with open(archivo_entrada, 'r', encoding='utf-8') as f:
            texto_plano = f.read()

        datos_bytes = texto_plano.encode('utf-8')

        # Generar IV
        iv = os.urandom(8)

        # Encriptar
        cipher = DES.new(llave_bytes, DES.MODE_CBC, iv)
        datos_con_padding = pad(datos_bytes, DES.block_size)
        texto_cifrado = cipher.encrypt(datos_con_padding)

        # Determinar ruta de salida
        if ruta_salida is None:
            # Usar el mismo nombre pero con extensión .enc
            archivo_salida = archivo_entrada.with_suffix('.enc')
        else:
            archivo_salida = _ruta_en_directorio_actual(ruta_salida)

        # Guardar resultado
        with open(archivo_salida, 'wb') as f:
            f.write(iv)
            f.write(texto_cifrado)

        print(f"Archivo encriptado guardado en: {archivo_salida.name}")
        print(f"Ruta completa: {archivo_salida}")
        print(f"IV usado: {iv.hex()}")
        print(f"Tamaño cifrado: {len(texto_cifrado)} bytes")

        return str(archivo_salida)

    except Exception as e:
        print(f"Error durante la encriptacion: {e}")
        return None

def descifrar_archivo(ruta_cifrado, llave_base64, ruta_salida=None):
    """Descifra un archivo .enc y guarda el resultado como .txt en el directorio actual"""
    try:
        # Decodificar llave
        llave_bytes = base64.b64decode(llave_base64)
        if len(llave_bytes) != 8:
            print(f"Error: La llave debe tener 8 bytes. Tiene: {len(llave_bytes)}")
            return None

        # Construir rutas en el directorio actual
        archivo_entrada = _ruta_en_directorio_actual(ruta_cifrado)

        # Verificar archivo cifrado
        if not archivo_entrada.exists():
            print(f"Error: El archivo {ruta_cifrado} no existe en {DIRECTORIO_ACTUAL}")
            return None

        # Leer archivo cifrado
        with open(archivo_entrada, 'rb') as f:
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

        # Determinar ruta de salida
        if ruta_salida is None:
            # Si el archivo de entrada era .enc, crear _descifrado.txt
            if archivo_entrada.suffix == '.enc':
                nombre_base = archivo_entrada.stem
                archivo_salida = DIRECTORIO_ACTUAL / f"{nombre_base}_descifrado.txt"
            else:
                archivo_salida = archivo_entrada.with_suffix('.txt')
        else:
            archivo_salida = _ruta_en_directorio_actual(ruta_salida)

        # Guardar archivo .txt
        with open(archivo_salida, 'w', encoding='utf-8') as f:
            f.write(texto_descifrado)

        print(f"Archivo descifrado guardado en: {archivo_salida.name}")
        print(f"Ruta completa: {archivo_salida}")
        if len(texto_descifrado) > 100:
            print(f"Contenido: {texto_descifrado[:100]}...")
        else:
            print(f"Contenido: {texto_descifrado}")

        return str(archivo_salida)

    except Exception as e:
        print(f"Error durante el descifrado: {e}")
        return None


def main() -> None:
    while True:
        _clean_console()
        print(f"""
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
                key = input("\nIngresa tu llave K: ")
                filename = input("Ingresa el nombre del archivo .txt: ")

                if not _file_exists(filename):
                    print(f"Error: El archivo {filename} no existe en la carpeta actual")
                    _wait_key()
                    continue

                filename_encrypt = input("Ingresa el nombre para el archivo cifrado (Enter para nombre automatico): ")
                if not filename_encrypt:
                    filename_encrypt = None

                encriptar_archivo_txt(filename, key, filename_encrypt)
                _wait_key()

            case "3":   
                key = input("\nIngresa tu llave K: ")
                filename = input("Ingresa el nombre del archivo cifrado (.enc): ")

                if not _file_exists(filename):
                    print(f"Error: El archivo {filename} no existe en la carpeta actual")
                    _wait_key()
                    continue

                filename_decrypt = input("Ingresa el nombre para el archivo descifrado (Enter para nombre automatico): ")
                if not filename_decrypt:
                    filename_decrypt = None

                descifrar_archivo(filename, key, filename_decrypt)
                _wait_key()
            case "4":
                break
            case _:
                print("Opcion no valida")
                _wait_key()

if __name__ == "__main__":
    main()