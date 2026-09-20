import logging
from pathlib import Path
from config import settings as cfg

BASE_DIR = Path(__file__).resolve().parent.parent
LOG_DIR = BASE_DIR / "logs"
LOG_DIR.mkdir(parents=True, exist_ok=True)
LOG_FILE = LOG_DIR / "app.log"


def setup_logger(name: str = "central") -> logging.Logger:
    """Configura e retorna um logger formatado gravando em arquivo e console."""
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG if cfg.DEBUG_MODE else logging.INFO)

    if not logger.handlers:
        # Formatador
        formatter = logging.Formatter("[%(asctime)s] [%(levelname)s] [%(name)s]: %(message)s", datefmt="%Y-%m-%d %H:%M:%S")

        # Handler de arquivo
        file_handler = logging.FileHandler(LOG_FILE, encoding="utf-8")
        file_handler.setFormatter(formatter)
        logger.addHandler(file_handler)

        # Handler de console
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)
        logger.addHandler(console_handler)

    return logger
