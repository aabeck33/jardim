import arrow
import requests
import sys
import os
import pytz
from rx import start
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from config import STORMGLASS_API_KEY, STORMGLASS_URL, LATITUDE, LONGITUDE

def get_weather():
    result = []
    # Define o fuso horário GMT-3
    tz = pytz.timezone("America/Bahia")
    startnow = arrow.now(tz)
    endnow = startnow.shift(hours=+3)

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
                "temp": hour.get("airTemperature", {}).get("noaa"),
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


print(get_weather())