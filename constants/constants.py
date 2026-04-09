from pathlib import Path

__all__ = [
    "BASE_DIR_CRYPTO",
    "RED",
    "GREEN",
    "YELLOW",
    "RESET",
    "SPANISH_ALPHABET",
]

# ==== Directorio base ====
BASE_DIR_CRYPTO = Path(__file__).resolve().parent.parent

# ==== Códigos ANSI ====
RED = "\033[91m"
GREEN = "\033[92m"
YELLOW = "\033[93m"
RESET = "\033[0m"

# ==== Alfabeto ====
SPANISH_ALPHABET = "ABCDEFGHIJKLMNÑOPQRSTUVWXY "
