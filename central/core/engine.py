import time
import config.settings as cfg
from hardware.lora.lora_receiver import LoRaRcvCont
from hardware.gpio_controller import init_gpio, aciona_bomba, cleanup_gpio
from services.weather_service import get_weather
from ia.decision_engine import decide_irrigation
from utils.helpers import parse_json_safely


class IrrigationEngine:
    def __init__(self):
        self.lora = LoRaRcvCont()
        self.ultimo_id = None

    def start(self):
        init_gpio()
        self.lora.start()
        print("🌱 [Core Engine] Sistema de Irrigação Central iniciado.")

        try:
            while True:
                msg = self.lora.receive()
                if msg is None:
                    break
                print("📡 Recebido via LoRa:", msg)
                sensor_data = parse_json_safely(msg)

                if sensor_data:
                    id_atual = sensor_data.get("id")
                    if id_atual is not None:
                        if self.ultimo_id is not None:
                            esperado = self.ultimo_id + 1
                            if id_atual != esperado:
                                print(
                                    f"⚠️ PERDA DETECTADA "
                                    f"(esperado={esperado}, recebido={id_atual})"
                                )
                        self.ultimo_id = id_atual
                          
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
