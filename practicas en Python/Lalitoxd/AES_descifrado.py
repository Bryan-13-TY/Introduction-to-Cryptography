from Crypto.Cipher import AES
from Crypto.Util import Counter
import base64

def descifrar_aes_ctr(cifrado_b64, clave_b64, iv_b64):
    clave = base64.b64decode(clave_b64)
    iv = base64.b64decode(iv_b64)
    cifrado = base64.b64decode(cifrado_b64)
    
    # Reconstruir el mismo contador que se usó en el cifrado
    contador = Counter.new(128, initial_value=int.from_bytes(iv, 'big'))
    cipher = AES.new(clave, AES.MODE_CTR, counter=contador)
    plaintext = cipher.decrypt(cifrado).decode('utf-8')
    return plaintext

if __name__ == "__main__":
    print("=== Descifrador AES-CTR ===")
    clave_b64 = input("Clave (base64): ").strip()
    iv_b64 = input("IV (base64): ").strip()
    cifrado_b64 = input("Cifrado (base64): ").strip()
    
    try:
        texto_recuperado = descifrar_aes_ctr(cifrado_b64, clave_b64, iv_b64)
        print(f"\nTexto descifrado: {texto_recuperado}")
    except Exception as e:
        print(f"Error al descifrar: {e}")
        
        