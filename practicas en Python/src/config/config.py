"""Configuración de rutas."""

from pathlib import Path

__all__ = ["BASE_DIR"]

# ==== Directorio base ====
ROOT_DIR = Path(__file__).resolve().parent.parent.parent

BASE_DIR = ROOT_DIR / "data"
