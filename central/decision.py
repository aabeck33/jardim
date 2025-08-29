import joblib
import pandas as pd
import arrow
import requests
import pytz
from config import STORMGLASS_API_KEY, STORMGLASS_URL, LATITUDE, LONGITUDE


def get_weather():
    """Busca a previsão do tempo no Stormglass.

    Args:
        None

    Raises:
        Exception: Erro ao buscar dados da API.

    Returns:
        dict: Um dicionário contendo as previsões climáticas.
                airTemperature, cloudCover, humidity, precipitation, 
                waterTemperature, windSpeed - Fonte: noaa
    """
    result = []
    # Define o fuso horário GMT-3
    tz = pytz.timezone("America/Bahia")
    startnow = arrow.now(tz)
    endnow = startnow.shift(hours=+2)   # Valores da hora atual e mais 2 (3 horas)

    try:
        headers = {"Authorization": STORMGLASS_API_KEY}
        params = {  "lat": LATITUDE, 
                    "lng": LONGITUDE, 
                    "params": "airTemperature,cloudCover,humidity,precipitation,waterTemperature,windSpeed", "source": "noaa",
                    "start": startnow.to('UTC').timestamp(),   # Convert to UTC timestamp
                    "end": endnow.to('UTC').timestamp()        # Convert to UTC timestamp
                  }
        response = requests.get(STORMGLASS_URL, headers=headers, params=params, timeout=5)
        data = response.json()
         # Verifica se há erro na resposta da API
        if "errors" in data:
            raise Exception(f" {data['errors'][list(data['errors'].keys())[0]]}")


        next_hours = data.get("hours", [])

        # Extrai os dados de cada hora
        for hour in next_hours:
            result.append({
                "airTemperature": hour.get("airTemperature", {}).get("noaa"),
                "cloudCover": hour.get("cloudCover", {}).get("noaa"),
                "humidity": hour.get("humidity", {}).get("noaa"),
                "precipitation": hour.get("precipitation", {}).get("noaa"),
                "waterTemperature": hour.get("waterTemperature", {}).get("noaa"),
                "windSpeed": hour.get("windSpeed", {}).get("noaa"),
                "time": hour.get("time")
            })
        return result
    except Exception as e:
        print("Erro da API Stormglass:", e)
        return {"temp": None, "cloudCover": None, "humidity": None, "precipitation": None, "waterTemperature": None, "windSpeed": None}

def decide_irrigation(sensor_data, weather_data):
    """Decide se liga ou não a bomba, e por quanto tempo.

    Args:
        sensor_data (dict): Dados dos sensores locais (ex: {'moisture': 45}).
        weather_data (dict): Dados do clima (ex: {'temp': 25, 'humidity': 60, 'rain': 0}).

    Returns:
        tuple: (bool, int) indicando se deve irrigar e o tempo em segundos.
    """
    # Carrega os modelos
    clf = joblib.load("modelo_class.pkl")
    reg = joblib.load("modelo_reg.pkl")

    X = pd.DataFrame([{
        "soil_moisture": sensor_data.get("moisture", 50),
        "airTemperature": wd.get("airTemperature", 25),
        "cloudCover": wd.get("cloudCover", 0),
        "humidity": wd.get("humidity", 50),
        "precipitation": wd.get("precipitation", 0),
        "waterTemperature": wd.get("waterTemperature", 25),
        "windSpeed": wd.get("windSpeed", 0)
    } for wd in weather_data])
    print(X)
    
    preds_class = clf.predict(X)
    preds_reg = reg.predict(X)

    deve_irrigar = int((preds_class == 1).all())
    tempo = int(preds_reg[preds_class == 1].max()) if deve_irrigar else 0

    return (bool(deve_irrigar), tempo)

print(decide_irrigation({"moisture": 10}, get_weather()))
# decision.py