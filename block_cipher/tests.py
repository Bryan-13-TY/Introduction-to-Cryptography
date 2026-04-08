from pathlib import Path
from tqdm import tqdm

BASE_DIR = Path(__file__).parent

def files_generator() -> None:
    with open(BASE_DIR / "test_prueba.txt", "w") as f:
        f.write("Rodrigo" * 10)  # 1000000

def main() -> None:
    files_generator()

if __name__ == "__main__":
    main()
