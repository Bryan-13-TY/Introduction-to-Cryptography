import msvcrt
import os

__all__ = [
    "clean_console",
    "wait_key",
]

def clean_console() -> None:
    os.system("cls" if os .name == "nt" else "clear")


def wait_key() -> None:
    print("\nPresiona enter para continuar...")
    msvcrt.getch()
