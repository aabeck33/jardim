import sys
from pathlib import Path

# Adiciona a raiz do projeto central ao sys.path
sys.path.append(str(Path(__file__).resolve().parent.parent))

from services import get_weather

if __name__ == "__main__":
    weather_data = get_weather()
    print("Previsão obtida:", weather_data)
