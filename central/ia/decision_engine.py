from pathlib import Path
import joblib
import pandas as pd
from config import settings as cfg

BASE_DIR = Path(__file__).resolve().parent
MODEL_CLASS_PATH = BASE_DIR / "models" / "modelo_class.pkl"
MODEL_REG_PATH = BASE_DIR / "models" / "modelo_reg.pkl"


def decide_irrigation(sensor_data: dict, weather_data: list | dict) -> tuple[bool, int]:
    """Decide se liga ou não a bomba, e por quanto tempo.

    Args:
        sensor_data (dict): Dados dos sensores locais (ex: {'moisture': 45}).
        weather_data (list|dict): Dados do clima obtidos do serviço de clima.

    Returns:
        tuple: (bool, int) indicando se deve irrigar e o tempo em segundos.
    """
    if not MODEL_CLASS_PATH.exists() or not MODEL_REG_PATH.exists():
        if cfg.DEBUG_MODE:
            print(f"[IA Warning] Modelos não encontrados em {MODEL_CLASS_PATH.parent}. Retornando padrão.")
        return (False, 0)

    clf = joblib.load(MODEL_CLASS_PATH)
    reg = joblib.load(MODEL_REG_PATH)

    if not isinstance(weather_data, list):
        weather_data = [weather_data] if weather_data else []

    if not weather_data:
        weather_data = [{"airTemperature": 25, "cloudCover": 50, "humidity": 60, "precipitation": 0, "waterTemperature": 22, "windSpeed": 5}]

    X = pd.DataFrame([{
        "soil_moisture": sensor_data.get("moisture", 50),
        "airTemperature": wd.get("airTemperature", 25),
        "cloudCover": wd.get("cloudCover", 0),
        "humidity": wd.get("humidity", 50),
        "precipitation": wd.get("precipitation", 0),
        "waterTemperature": wd.get("waterTemperature", 25),
        "windSpeed": wd.get("windSpeed", 0)
    } for wd in weather_data])

    if cfg.DEBUG_MODE:
        print("[IA Input Matrix]:\n", X)
    
    preds_class = clf.predict(X)
    preds_reg = reg.predict(X)

    deve_irrigar = int((preds_class == 1).all())
    tempo = int(preds_reg[preds_class == 1].max()) if deve_irrigar else 0

    return (bool(deve_irrigar), tempo)
