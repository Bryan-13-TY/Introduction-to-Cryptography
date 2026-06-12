import time 
from Crypto.Util import number  # LA ÚNICA LIBRERÍA CRIPTOGRÁFICA PERMITIDA

ejercicio_1_datos = [
    [11, 1009, 400],                   # Inciso a
    [5, 10007, 5235],                  # Inciso b
    [2, 100000000003, 1922556950],     # Inciso c
    [3, 500000009, 406870124],         # Inciso d
    [3, 500000009, 187776257]          # Inciso e
]

ejercicio_2_datos = [
    [100160063],                       # Inciso a
    [10006200817],                     # Inciso b
    [250035001189],                    # Inciso c
    [250000009000000081]               # Inciso d
]

def resolver_logaritmo_discreto(base, modulo, objetivo):
    for x in range(modulo):
        if pow(base, x, modulo) == objetivo:
            return x
    return None


def factorizar_n(n):
    limite = int(n**0.5) + 1
    for p in range(2, limite):
        if n % p == 0:
            q = n // p
            return p, q
    return None

def generar_primos_criptograficos():
    primos = []
    for _ in range(10):
        p = number.getPrime(512)
        primos.append(p)
    return primos

if __name__ == "__main__":
    
    print(" EJERCICIO 1: LOGARITMO DISCRETO (FUERZA BRUTA) ")
    
    for i, datos in enumerate(ejercicio_1_datos):
        base, modulo, objetivo = datos[0], datos[1], datos[2]
        
        print(f"\n{i+1}. {base}^x mód {modulo} = {objetivo} ")
        
        inicio = time.time()
        x_encontrada = resolver_logaritmo_discreto(base, modulo, objetivo)
        fin = time.time()
        
        tiempo_total = fin - inicio
        print(f"-> Resultado: x = {x_encontrada}")
        print(f"-> Tiempo requerido: {tiempo_total:.6f} segundos")
        
    print(" EJERCICIO 2: FACTORIZACIÓN DE N (p * q) ")
    
    for i, datos in enumerate(ejercicio_2_datos):
        n = datos[0]
        
        print(f"\nFactorizando inciso {i+1}) n = {n}...")
        
        inicio = time.time()
        resultado = factorizar_n(n)
        fin = time.time()
        
        tiempo_total = fin - inicio
        if resultado:
            p, q = resultado
            print(f"Factores encontrados: p = {p}, q = {q}")
        else:
            print("-> No se encontraron factores.")
        print(f"-> Tiempo requerido: {tiempo_total:.6f} segundos")


    print(" EJERCICIO 3: GENERACIÓN DE 10 PRIMOS (512 BITS) ")

    inicio_global = time.time()
    for i in range(1, 11):
        inicio_primo = time.time()
        primo_grande = number.getPrime(512)
        fin_primo = time.time()
        
        print(f"\nPrimo #{i} (512 bits):")
        print(f"{primo_grande}")
        print(f"Tiempo de generación: {fin_primo - inicio_primo:.6f} segundos")
        
    fin_global = time.time()
    print(f"\nTiempo total para los 10 primos: {fin_global - inicio_global:.6f} segundos")