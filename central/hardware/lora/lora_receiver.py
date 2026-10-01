import json
import threading
import time
from queue import Empty, Queue
import config.settings as cfg
from . import lora_ctrl as loractrl


class LoRaRcvCont:
    MAX_MESSAGE_BYTES = 4096

    def __init__(self, mode: str | None = None):
        """Inicializa a classe para recepção contínua de dados via LoRa (módulo E220)."""
        self.mode = mode or cfg.CENTRAL_MODE
        if self.mode not in {"teste", "teste_radio", "producao"}:
            raise ValueError(
                "mode deve ser 'teste', 'teste_radio' ou 'producao'."
            )
        self.ser = None
        self.thread = None
        self.running = False
        self.messages = Queue()
        self.stop_event = threading.Event()
        self.buffer = bytearray()
        self.test_sequence = 0

    def _setup_radio(self):
        """Configura os pinos GPIO e inicializa a conexão serial."""
        if self.mode == "teste":
            return

        if cfg.DEBUG_MODE:
            print("[LoRa] Configurando rádio e porta serial...")
            
        loractrl.set_mode("normal")
        
        try:
            import serial

            self.ser = serial.Serial(
                cfg.PORT, 
                baudrate=cfg.BAUDRATE, 
                timeout=1, 
                bytesize=8, 
                parity='N', 
                stopbits=1
            )
            if cfg.DEBUG_MODE:
                print(f"[LoRa] Porta {cfg.PORT} aberta com sucesso.")
                #loractrl.read_parameters(self.ser)
        except Exception as e:
            raise RuntimeError(
                f"Não foi possível abrir a porta LoRa {cfg.PORT}: {e}"
            ) from e

        time.sleep(0.2)

    def _queue_complete_lines(self, data: bytes):
        self.buffer.extend(data)
        if len(self.buffer) > self.MAX_MESSAGE_BYTES and b"\n" not in self.buffer:
            print("[LoRa] Mensagem excedeu o limite e foi descartada.")
            self.buffer.clear()
            return

        while b"\n" in self.buffer:
            raw_message, _, remainder = self.buffer.partition(b"\n")
            self.buffer = bytearray(remainder)
            if len(raw_message) > self.MAX_MESSAGE_BYTES:
                print("[LoRa] Mensagem excedeu o limite e foi descartada.")
                continue
            try:
                message = raw_message.decode("utf-8").strip()
            except UnicodeDecodeError:
                print("[LoRa] Mensagem descartada: conteúdo não é UTF-8 válido.")
                continue
            if message:
                self.messages.put(message)
                if cfg.DEBUG_MODE:
                    print(f"📡 [LoRa Thread] Nova mensagem: {message}")

        if len(self.buffer) > self.MAX_MESSAGE_BYTES:
            print("[LoRa] Mensagem excedeu o limite e foi descartada.")
            self.buffer.clear()

    def _listen_loop(self):
        """Loop principal da thread que lê a serial continuamente."""
        if self.mode == "teste":
            self._test_listen_loop()
            return

        if cfg.DEBUG_MODE:
            print("[LoRa Thread] Iniciando loop de recepção...")
            print(self.ser.port if self.ser else "Serial não inicializada.")
            print(self.ser.baudrate if self.ser else "Serial não inicializada.")
        
        while self.running:
            try:
                if self.ser and self.ser.is_open and self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    self._queue_complete_lines(data)
                else:
                    time.sleep(0.05)
            except Exception as e:
                if cfg.DEBUG_MODE:
                    print(f"[LoRa Error] Falha na leitura: {e}")
                time.sleep(1)

    def _test_listen_loop(self):
        while self.running:
            self.test_sequence += 1
            payload = {
                "dispositivo": "esp32-teste-01",
                "tipo": "ESP32V3",
                "versao": "simulacao",
                "protocolo_telemetria": 1,
                "id": self.test_sequence,
                "timestamp": self.test_sequence * 5000,
                "sensores": [
                    {
                        "id": "solo_1",
                        "tipo": "umidade_solo",
                        "estado": "ok",
                        "valor_raw": 2800,
                        "valor_calibrado": 32.5,
                        "unidade": "percent",
                    },
                    {
                        "id": "temperatura_interna",
                        "tipo": "temperatura_interna",
                        "estado": "ok",
                        "valor_calibrado": 27.0,
                        "unidade": "celsius",
                    },
                ],
            }
            self.messages.put(json.dumps(payload))
            self.stop_event.wait(cfg.TEST_MESSAGE_INTERVAL)

    def start(self):
        """Inicia a thread de recepção."""
        if not self.running:
            self._setup_radio()
            self.stop_event.clear()
            self.buffer.clear()
            self.running = True
            self.thread = threading.Thread(target=self._listen_loop, daemon=True, name="LoRaListenThread")
            self.thread.start()
            if cfg.DEBUG_MODE:
                print("[LoRa] Thread de recepção iniciada em segundo plano.")

    def receive(self, timeout: float = 1):
        """Aguarda uma mensagem até o timeout sem encerrar o receptor."""
        try:
            return self.messages.get(timeout=timeout)
        except Empty:
            return None

    def stop(self):
        """Finaliza a thread e fecha a serial."""
        if cfg.DEBUG_MODE:
            print("[LoRa] Encerrando recepção...")
        self.running = False
        self.stop_event.set()
        if self.thread:
            self.thread.join(timeout=2)
        if self.ser and self.ser.is_open:
            self.ser.close()
        if cfg.DEBUG_MODE:
            print("[LoRa] Recursos liberados.")
