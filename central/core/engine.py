import config.settings as cfg
from hardware.lora.lora_receiver import LoRaRcvCont
from hardware.gpio_controller import (
    init_gpio,
    init_lora_gpio,
    aciona_bomba,
    cleanup_gpio,
)
from services.weather_service import get_weather
from ia.decision_engine import decide_irrigation
from utils.helpers import parse_json_safely


class IrrigationEngine:
    def __init__(self):
        self.lora = LoRaRcvCont()
        self.ultimo_id = {}
        self.gpio_initialized = False

    def start(self):
        try:
            if cfg.CENTRAL_MODE == "producao":
                init_gpio()
                self.gpio_initialized = True
            elif cfg.CENTRAL_MODE == "teste_radio":
                init_lora_gpio()
                self.gpio_initialized = True
            self.lora.start()
            print(
                f"🌱 [Core Engine] Central iniciada em modo {cfg.CENTRAL_MODE}."
            )

            while True:
                msg = self.lora.receive()
                if msg is None:
                    continue
                print("📡 Recebido via LoRa:", msg)
                sensor_data = parse_json_safely(msg)

                if not isinstance(sensor_data, dict):
                    print("[LoRa] Pacote JSON inválido ou formato inesperado.")
                    continue

                self._check_sequence(sensor_data)
                if sensor_data.get("moisture") is None:
                    print(
                        "[Central] Telemetria recebida; decisão ignorada porque "
                        "o pacote ainda não contém o campo legado 'moisture'."
                    )
                    continue

                weather = (
                    get_weather()
                    if cfg.CENTRAL_MODE == "producao"
                    else [{
                        "airTemperature": 25,
                        "cloudCover": 50,
                        "humidity": 60,
                        "precipitation": 0,
                        "waterTemperature": 22,
                        "windSpeed": 5,
                    }]
                )
                irrigar, tempo = decide_irrigation(sensor_data, weather)
                if irrigar and cfg.CENTRAL_MODE == "producao":
                    aciona_bomba(tempo)
                elif irrigar:
                    print(f"[Teste] Acionamento simulado por {tempo} segundos.")
                else:
                    print("✅ Não é necessário irrigar agora.")
        except KeyboardInterrupt:
            print("Encerrado pelo usuário.")
        finally:
            self.stop()

    def _check_sequence(self, sensor_data):
        device_id = sensor_data.get("dispositivo", "desconhecido")
        current_id = sensor_data.get("id")
        if not isinstance(current_id, int):
            return

        previous_id = self.ultimo_id.get(device_id)
        if previous_id is not None and current_id != previous_id + 1:
            print(
                f"⚠️ PERDA DETECTADA em {device_id} "
                f"(esperado={previous_id + 1}, recebido={current_id})"
            )
        self.ultimo_id[device_id] = current_id

    def stop(self):
        self.lora.stop()
        if self.gpio_initialized:
            cleanup_gpio()
        print("🛑 [Core Engine] Sistema encerrado com segurança.")
