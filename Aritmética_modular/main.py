def es_primo(numero: int) -> bool:    
    for i in range(1, numero + 1):
        if i == 1 or i == numero:
            continue
        if numero % i == 0:
            return False
    
    return True


def generar_primos(tope: int) -> list[int]:
    numeros = [i for i in range(1, tope + 1) if not (i == 1 or i == tope)]
    son_primos = {f"{i}": es_primo(i) for i in numeros}
    primos = [int(i) for i in son_primos.keys() if son_primos[i]]
    return primos


def calcular_modulo(numero: int, modulo: int) -> int:
    return numero % modulo


def main() -> None:
    primos = generar_primos(4376)
    print(*primos)

def fib(n: int) -> None:
    a, b = 0, 1
    while a < n:
        print(a, end=" ")
        a, b = b, a + b
    print()


if __name__ == "__main__":
    main()