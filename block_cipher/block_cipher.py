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
    # Generar 8 bytes aleatorios
    llave_bytes = np.random.bytes(8)
    
    # Convertir a base64
    llave_base64 = base64.b64encode(llave_bytes).decode()
    
    print(f"🔑 Llave DES generada (base64): {llave_base64}")
    print(f"   Bytes: {' '.join(f'{b:02X}' for b in llave_bytes)}")
    
    return llave_base64

def encriptar_archivo_txt(ruta_txt, llave_base64, ruta_salida=None):
    """Encripta un archivo .txt usando DES"""
    try:
        # 1. Procesar la llave - ¡CORREGIDO!
        llave_bytes = base64.b64decode(llave_base64)
        
        # Verificar que la llave tiene 8 bytes
        if len(llave_bytes) != 8:
            print(f"❌ Error: La llave debe tener 8 bytes. Tiene: {len(llave_bytes)}")
            return None
        
        # 2. Verificar que el archivo de entrada existe
        if not Path(ruta_txt).exists():
            print(f"❌ Error: El archivo {ruta_txt} no existe")
            return None
        
        # 3. Leer el archivo .txt
        with open(ruta_txt, 'r', encoding='utf-8') as f:
            texto_plano = f.read()
        
        datos_bytes = texto_plano.encode('utf-8')
        
        # 4. Generar IV aleatorio
        iv = os.urandom(8)
        
        # 5. Encriptar - ¡CORREGIDO! (usar llave_bytes, no llave_base64)
        cipher = DES.new(llave_bytes, DES.MODE_CBC, iv)
        datos_con_padding = pad(datos_bytes, DES.block_size)
        texto_cifrado = cipher.encrypt(datos_con_padding)
        
        # 6. Guardar resultado (IV + datos cifrados)
        if ruta_salida is None:
            ruta_salida = str(Path(ruta_txt).stem) + '.enc'
        
        with open(ruta_salida, 'wb') as f:
            # Guardamos el IV al principio del archivo (necesario para descifrar)
            f.write(iv)
            f.write(texto_cifrado)
        
        print(f"✅ Archivo encriptado guardado en: {ruta_salida}")
        print(f"   IV usado: {iv.hex()}")
        print(f"   Tamaño cifrado: {len(texto_cifrado)} bytes")
        
        return ruta_salida
        
    except Exception as e:
        print(f"❌ Error durante la encriptación: {e}")
        return None

def descifrar_archivo(ruta_cifrado, llave_base64, ruta_salida=None):
    """Descifra un archivo .enc y guarda el resultado como .txt"""
    try:
        # 1. Procesar la llave - ¡CORREGIDO!
        llave_bytes = base64.b64decode(llave_base64)
        
        # Verificar que la llave tiene 8 bytes
        if len(llave_bytes) != 8:
            print(f"❌ Error: La llave debe tener 8 bytes. Tiene: {len(llave_bytes)}")
            return None
        
        # 2. Verificar que el archivo cifrado existe
        if not Path(ruta_cifrado).exists():
            print(f"❌ Error: El archivo {ruta_cifrado} no existe")
            return None
        
        # 3. Leer archivo cifrado
        with open(ruta_cifrado, 'rb') as f:
            # Los primeros 8 bytes son el IV
            iv = f.read(8)
            texto_cifrado = f.read()
        
        # 4. Descifrar - ¡CORREGIDO! (usar llave_bytes, no llave_base64)
        cipher = DES.new(llave_bytes, DES.MODE_CBC, iv)
        datos_descifrados = cipher.decrypt(texto_cifrado)
        
        # 5. Quitar padding
        try:
            datos_sin_padding = unpad(datos_descifrados, DES.block_size)
        except ValueError as e:
            print(f"❌ Error de padding: {e}")
            print("   Posiblemente la llave es incorrecta o el archivo está corrupto")
            return None
        
        # 6. Convertir bytes a texto
        texto_descifrado = datos_sin_padding.decode('utf-8')
        
        # 7. Guardar archivo .txt
        if ruta_salida is None:
            ruta_salida = str(Path(ruta_cifrado).stem) + '_descifrado.txt'
        
        with open(ruta_salida, 'w', encoding='utf-8') as f:
            f.write(texto_descifrado)
        
        print(f"✅ Archivo descifrado guardado en: {ruta_salida}")
        print(f"   Contenido: {texto_descifrado[:100]}..." if len(texto_descifrado) > 100 else f"   Contenido: {texto_descifrado}")
        
        return ruta_salida
        
    except Exception as e:
        print(f"❌ Error durante el descifrado: {e}")
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
        option = input("Opción: ")
        
        match option:
            case "1":
                key_random_generator()
                _wait_key()
                
            case "2":
                key = input("Ingresa tu llave K: ")
                filename = input("Ingresa el nombre de tu archivo: ")
                
                if not _file_exists(filename):
                    print("❌ El archivo no existe")
                    _wait_key()
                    continue
                
                filename_encrypt = input("Ingresa el nombre con el que se guardará tu archivo cifrado (Enter para nombre automático): ")
                if not filename_encrypt:
                    filename_encrypt = None
                
                encriptar_archivo_txt(filename, key, filename_encrypt)
                _wait_key()
                
            case "3":
                key = input("Ingresa tu llave K: ")
                filename = input("Ingresa el nombre de tu archivo cifrado: ")
                
                if not _file_exists(filename):
                    print("❌ El archivo cifrado no existe")
                    _wait_key()
                    continue
                
                filename_decrypt = input("Ingresa el nombre con el que se guardará tu archivo descifrado (Enter para nombre automático): ")
                if not filename_decrypt:
                    filename_decrypt = None
                
                descifrar_archivo(filename, key, filename_decrypt)
                _wait_key()
                
            case "4":
                print("👋 ¡Hasta luego!")
                break
                
            case _:
                print("❌ Opción no válida")
                _wait_key()

if __name__ == "__main__":
    main()