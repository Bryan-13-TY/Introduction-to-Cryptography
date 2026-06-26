from time import perf_counter
from math import isqrt
from Crypto.Util.number import getPrime

d = [
    (11, 1009, 400),
    (5, 10007, 5235),
    (2, 100000000003, 1922556950),
    (3, 500000009, 406870124),
    (3, 500000009, 187776257),
]

numbers = [
    100_160_063,
    10_006_200_817,
    250_035_001_189,
    250_000_009_000_000_081,
]

def brute_force_discrete_log(
        base: int,
        target: int,
        modulus: int
) -> tuple[int | None, float, float]:
    start = perf_counter()

    solution = None
    
    for x in range(modulus):
        if (base * x) % modulus == target:
            solution = x
            break

    end = perf_counter()

    return solution, start, end


def factor_semiprime(
        n: int
) -> tuple[int, int, float, float] | None:
    limit = isqrt(n)
    start = perf_counter()

    for p in range(2, limit + 1):
        if n % p == 0:
            q = n // p
            end = perf_counter()
            return p, q, start, end
        
    return None


def generate_prime_512():
    for i in range(10):
        start = perf_counter()
        prime = getPrime(512)
        print(f"Prime {i + 1}:")
        print(f"Bits: {prime.bit_length()}")
        print(prime)
        end = perf_counter()
        print(f"Tiempo: {end - start:.8f} segundos\n")
        print()


def main() -> None:
    print("Ejrcicio 1:\n")
    for i in d:
        base, modulus, target = i
        solution, start, end = brute_force_discrete_log(base, target, modulus)
        print(f"{base}^{solution} mod {modulus} = {target}")
        print(f"Tiempo: {end - start:.8f} segundos\n")

    print("Ejercicio 2:\n")
    for n in numbers:
        factors = factor_semiprime(n)
        
        if factors:
            p, q, start, end = factors
            print(f"{n} = {p} x {q}")
            print(f"Tiempo: {end - start:.8f} segundos\n")
        else:
            print(f"No se encontraron factores para {n}")
    
    print("\nEjercicio 3:\n")
    generate_prime_512()

if __name__ == "__main__":
    main()
