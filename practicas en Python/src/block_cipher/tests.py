"""Generación de archivo de prueba para DES."""

from config import BASE_DIR
from utils import (
    yellow,
    success,
)

__all__ = ["file_generator_DES"]

def file_generator_DES() -> None:
    with open(BASE_DIR / "test_prueba_DES.txt", "w") as f:
        f.write("Rodrigo" * 1000000)  # 1000000

    print(f"\n{yellow('>>')} {success('El archivo se genero correctamente')}")

def main() -> None:
    file_generator_DES()

if __name__ == "__main__":
    main()
