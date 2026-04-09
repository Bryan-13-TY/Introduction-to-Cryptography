from pathlib import Path

__all__ = [
    "BASE_DIR_CRYPTO",
    "RED",
    "GREEN",
    "YELLOW",
    "RESET",
]

# ==== Directorio base ====
BASE_DIR_CRYPTO = Path(__file__).resolve().parent.parent

# ==== Códigos ANSI ====
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
RESET = "\033[0m"
