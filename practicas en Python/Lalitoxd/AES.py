from Crypto.Random import get_random_bytes
import base64

def generar_clave(bits):
    bytes_len = bits // 8
    clave = get_random_bytes(bytes_len)
    return base64.b64encode(clave).decode('utf-8')

if __name__ == "__main__":
    for bits in [128, 192, 256]:
        clave_b64 = generar_clave(bits)
        print(f"Clave AES-{bits}: {clave_b64}")
        