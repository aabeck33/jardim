import requests
import arrow
import pytz
from config import settings as cfg


def get_weather() -> list[dict]:
    """Busca a previsão do tempo no Stormglass API.

    Returns:
        list[dict]: Uma lista contendo previsões climáticas com campos:
                    airTemperature, cloudCover, humidity, precipitation, 
                    waterTemperature, windSpeed.
    """
    result = []
    tz = pytz.timezone("America/Bahia")
    startnow = arrow.now(tz)
    endnow = startnow.shift(hours=+2)

    try:
        headers = {"Authorization": cfg.STORMGLASS_API_KEY}
        params = {
            "lat": cfg.LATITUDE, 
            "lng": cfg.LONGITUDE, 
            "params": "airTemperature,cloudCover,humidity,precipitation,waterTemperature,windSpeed", 
            "source": "noaa",
            "start": startnow.to('UTC').timestamp(),
            "end": endnow.to('UTC').timestamp()
        }
        response = requests.get(cfg.STORMGLASS_URL, headers=headers, params=params, timeout=5)
        data = response.json()
        
        if "errors" in data:
            raise Exception(f" {data['errors'][list(data['errors'].keys())[0]]}")

        next_hours = data.get("hours", [])

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
        if cfg.DEBUG_MODE:
            print("Erro na API Stormglass:", e)
        # Fallback de segurança com valores padrão
        return [{
            "airTemperature": 25.0,
            "cloudCover": 50.0,
            "humidity": 60.0,
            "precipitation": 0.0,
            "waterTemperature": 22.0,
            "windSpeed": 5.0,
            "time": arrow.now().isoformat()
        }]
