import config as cfg
import time
import serial
import requests
import threading
import json
import RPi.GPIO as GPIO
from lora_receiver import LoRaRcvCont
from weather_api import get_weather
from central.decision import decide_irrigation


def aciona_bomba(segundos):
    print(f"💧 Ligando bomba por {segundos} segundos...")
    GPIO.output(cfg.BOMBA_PIN, GPIO.HIGH)
    time.sleep(segundos)
    GPIO.output(cfg.BOMBA_PIN, GPIO.LOW)
    print("💧 Bomba desligada.")

def main():
    lora = LoRaRcvCont()
    lora.start()

    while True:
        msg = lora.receive()
        if msg:
            print("📡 Recebido via LoRa:", msg)
            try:
                sensor_data = json.loads(msg)  # ESP32 manda em JSON
            except:
                print("Erro ao decodificar JSON LoRa.")
                continue

            # Clima
            weather = get_weather()
            print("🌤️ Clima atual:", weather)

            # Decisão
            irrigar, tempo = decide_irrigation(sensor_data, weather)
            if irrigar:
                aciona_bomba(tempo)
            else:
                print("✅ Não é necessário irrigar agora.")

        time.sleep(2)

if __name__ == "__main__":
    try:
        if cfg.DEBUG_MODE:
            print("Configurando GPIO...")
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(cfg.BOMBA_PIN, GPIO.OUT)
        GPIO.output(cfg.BOMBA_PIN, GPIO.LOW)

        main()
    except KeyboardInterrupt:
        print("Encerrado pelo usuário.")
    finally:
        GPIO.cleanup()
