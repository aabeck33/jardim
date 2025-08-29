import random
import csv

with open('dados_irrigacao_sintetico.csv', 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['soil_moisture','airTemperature','cloudCover','humidity','precipitation','waterTemperature','windSpeed','irrigar','tempo_bomba'])
    for _ in range(300):
        soil_moisture = random.randint(10, 60)
        airTemperature = random.randint(10, 36)
        cloudCover = random.randint(0, 100)
        humidity = random.randint(40, 90)
        #precipitation = random.randint(0, 5)
        precipitation = 0
        waterTemperature = random.randint(20, 40)
        windSpeed = random.randint(0, 5)
        print(soil_moisture, airTemperature, cloudCover, humidity, precipitation, waterTemperature, windSpeed)
        # Regras para irrigar
        if soil_moisture < 30 and airTemperature > 20 and precipitation == 0:
            irrigar = 1
            tempo_bomba = random.randint(10, 20)
        elif precipitation > 2 or humidity > 80:
            irrigar = 0
            tempo_bomba = 0
        else:
            irrigar = random.choice([0, 1])
            tempo_bomba = random.randint(5, 15) if irrigar else 0

        writer.writerow([soil_moisture, airTemperature, cloudCover, humidity, precipitation, waterTemperature, windSpeed, irrigar, tempo_bomba])
print("Dados sintéticos gerados em 'dados_irrigacao_sintetico.csv'")