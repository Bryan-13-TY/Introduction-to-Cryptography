import random
from Crypto.Util import number

BITS = [
    [16],                  
    [32],                  
    [512],     
    [2048]       
]

def generar_primos_criptograficos():
     for i,datos in enumerate(BITS):
        n = datos[0]
        primo_grande = number.getPrime(n)
        
        print(f"\nPrimo #{i} ({n} bits):")
        print(f"{primo_grande}")

def generate_key_pair_RSA(bits):
    p = number.getPrime(bits)
    q = number.getPrime(bits)
    
    if abs(p - q) < 2:  # Asegurarse de que p y q no sean iguales
        q = number.getPrime(bits)
        
    print(f"\nPrimo p de ({p.bit_length()} bits): {p}")
    print(f"Primo q de ({q.bit_length()} bits): {q}")
    
    n = p * q
    phi = (p - 1) * (q - 1)
    
    e = random.randint(2, phi - 1) 
    while number.GCD(e, phi) != 1: 
        e = random.randint(2, phi - 1)
    d = number.inverse(e, phi)
    
    return (e, n), (d, n)

if __name__ == "__main__":

    generar_primos_criptograficos()
    
    print("\nCLAVE RSA")
    bits = int(input("\nDigite el número de bits para la clave RSA: "))
    public_key, private_key = generate_key_pair_RSA(bits)

    print("\nClave Pública: ", public_key)
    print("Clave Privada: ", private_key)
        
