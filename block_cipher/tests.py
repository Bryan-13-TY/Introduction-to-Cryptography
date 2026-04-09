from constants import BASE_DIR_CRYPTO

__all__ = ["file_generator_DES"]

def file_generator_DES() -> None:
    with open(BASE_DIR_CRYPTO / "test_prueba_DES.txt", "w") as f:
        f.write("Rodrigo" * 10)  # 1000000

def main() -> None:
    file_generator_DES()

if __name__ == "__main__":
    main()
