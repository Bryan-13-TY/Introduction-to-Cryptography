"""Cifrado usando RSA."""

from math import gcd
from secrets import (
    randbelow,
    randbits,
)

from Crypto.Util.number import getPrime

from config import BASE_DIR
from utils import (
    clean_console,
    wait_key,
    error,
    success,
    yellow,
)

__all__ = [
    "rsa_cipher_menu",
    "get_primes",
]

BIT_SIZES = [16, 32, 512, 2048]

def get_primes() -> None:
    for bits in BIT_SIZES:
        prime = getPrime(bits)

        print(f"\nPrimo de {bits} bits:", prime)
        print(f"Tamaño en bits: {prime.bit_length()}")


def _primes_generator(bits: int) -> tuple[int, int]:
    """
    Genera dos números primos aleatorios `p` y `q` distintos de
    `bits` tamaño.

    :param bits: Tamaño de los números primos.
    :type bits: int
    :return: Números primos `p` y `q`.
    :rtype: tuple[int, int]
    """
    p = getPrime(bits)

    while True:
        q = getPrime(bits)

        if q != p:
            return p, q


def _get_ramdom_public_exponent(phi: int) -> int:
    while True:
        e = randbelow(phi - 2) + 2

        if e in (3, 65537):
            continue

        if gcd(e, phi) == 1:
            return e


def _random_keypair_generator(bits: int) -> tuple[tuple[int, int], int]:
    p, q = _primes_generator(bits)

    n = p * q
    phi = (p - 1) * (q - 1)
    e = _get_ramdom_public_exponent(phi)
    d = pow(e, -1, phi)

    return (e, n), d


def _store_keypair(public_key: tuple[int, int], d: int) -> None:
    """
    Almacena la llave pública y la llave privada en un archivo.

    :param public_key: Llave pública (e, n).
    :type public_key: tuple[int, int]
    :param d: Llave privada.
    :type d: int
    """
    with open(BASE_DIR / "keys.key", "w", encoding="utf-8") as f:
        e, n = public_key
        f.write(f"e={e}\n")
        f.write(f"d={d}\n")
        f.write(f"n={n}\n")

    print(
        f"\n{yellow('>>')} "
        f"{success('Llave pública y privada guardadas correctamente como keys.key')}"
    )


def _encrypt(e: int, n: int) -> None:
    """
    Cifra un valor aleatorio de 16 bits.

    :param e: Exponente público.
    :type e: int
    :param n: Módulo.
    :type n: int
    """
    r = randbits(15) | (1 << 15)

    c = pow(r, e, n)

    print(f"\nr = {r}")
    print(f"c = {c}")


def _decrypt(d: int, n: int, c: int) -> None:
    """
    Descifra `c`.    

    :param d: Exponente privado.
    :type d: int
    :param n: Módulo.
    :type n: int
    :param c: Ciphertext.
    :type c: int
    """
    m = pow(c, d, n)

    print(f"\nValor recuperado: {m}")


def rsa_cipher_menu() -> None:
    while True:
        clean_console()
        print(f"""
/*-----------.
| RSA CIPHER |
`-----------*/

{yellow('>>')} Elija una de las opciones

1.- Crear una llave para RSA
2.- Cifrar un archivo
3.- Descifrar un archivo
4.- Salir
""")
        option = input("Opción: ")
        match option:
            case "1":
                try:
                    bits = int(input("\nEscribe un número de bits (32, 512 o 2048 bits): "))
                except ValueError:
                    print(
                        f"\n{yellow('>>')} {error('ERROR')}"
                        ": Debe ser un número"
                    )
                    wait_key()
                    continue

                if bits not in BIT_SIZES:
                    print(
                        f"{yellow('>>')} {error('ERROR')}"
                        ": Solo se aceptan 32, 512 o 2048 bits"
                    )
                    continue

                public_key, d = _random_keypair_generator(bits)
                e, n = public_key

                print(f"\n{yellow('>>')} Llave pública")
                print(f"e = {e}")
                print(f"n = {n}")

                print(f"\n{yellow('>>')} Exponente privado")
                print(f"d = {d}")
                _store_keypair(public_key, d)
                wait_key()
            case "2":
                try:
                    e = int(input("\nEscribe el valor de e: "))
                    n = int(input("Escribe el valor de n: "))
                except ValueError:
                    print(
                        f"\n{yellow('>>')} {error('ERROR')}"
                        ": Debe ser un número"
                    )
                    wait_key()
                    continue

                _encrypt(e, n)
                wait_key()
            case "3":
                try:
                    d = int(input("\nEscribe el valor de d: "))
                    n = int(input("Escribe el valor de n: "))
                    c = int(input("Escribe el valor de c: "))
                except ValueError:
                    print(
                        f"\n{yellow('>>')} {error('ERROR')}"
                        ": Debe ser un número"
                    )
                    wait_key()
                    continue

                _decrypt(d, n, c)
                wait_key()
            case "4":
                break
            case _:
                print(f"\n{yellow('>>')} {error('ERROR')}: Opción no válida")
                wait_key()


def main() -> None:
    rsa_cipher_menu()

if __name__ == "__main__":
    main()
