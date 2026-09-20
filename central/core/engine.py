import time
from config import settings as cfg
from hardware.lora import LoRaRcvCont
from hardware.gpio_controller import init_gpio, aciona_bomba, cleanup_gpio
from services import get_weather
from ia import decide_irrigation
from utils import parse_json_safely


class IrrigationEngine:
    def __init__(self):
        self.lora = LoRaRcvCont()

    def start(self):
        init_gpio()
        self.lora.start()
        print("🌱 [Core Engine] Sistema de Irrigação Central iniciado.")

        try:
            while True:
                msg = self.lora.receive()
                if msg:
                    print("📡 Recebido via LoRa:", msg)
                    sensor_data = parse_json_safely(msg)
                    if not sensor_data:
                        print("Erro ao decodificar JSON LoRa.")
                        continue

                    weather = get_weather()
                    print("🌤️ Clima atual:", weather)

                    irrigar, tempo = decide_irrigation(sensor_data, weather)
                    if irrigar:
                        aciona_bomba(tempo)
                    else:
                        print("✅ Não é necessário irrigar agora.")

                time.sleep(2)
        except KeyboardInterrupt:
            print("Encerrado pelo usuário.")
        finally:
            self.stop()

    def stop(self):
        self.lora.stop()
        cleanup_gpio()
        print("🛑 [Core Engine] Sistema encerrado com segurança.")
