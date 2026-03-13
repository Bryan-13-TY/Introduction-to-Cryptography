from pathlib import Path

BASE_DIR = Path(__file__).parent

def files_generator() -> None:
    with open(BASE_DIR / "test.txt", "w") as f:
        f.write("A" * 200000)

def main() -> None:
    files_generator()

if __name__ == "__main__":
    main()