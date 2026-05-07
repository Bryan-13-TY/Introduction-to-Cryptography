"""Funciones para dar color al texto."""

from colorama import (
    Fore,
    Style,
    init,
)

__all__ = [
    "error",
    "success",
    "yellow",
]

init(autoreset=True)

RESET = Style.RESET_ALL

def error(text: str) -> str:
    """Muestra un error en color rojo."""
    return Fore.RED + Style.BRIGHT + text + RESET


def success(text: str) -> str:
    """Muestra un mensaje de éxito en color verde."""
    return Fore.GREEN + Style.BRIGHT + text + RESET


def yellow(text: str) -> str:
    """Muestra un mensaje de color amarillo."""
    return Fore.YELLOW + text + RESET
