import threading
from queue import Queue
import time
import serial
import config.settings as cfg
from . import lora_ctrl as loractrl

class LoRaRcvCont:
    def __init__(self):
        """Inicializa a classe para recepção contínua de dados via LoRa (módulo E220)."""
        self.ser = None
        self.thread = None
        self.running = False
        self.messages = Queue()
        self.lock = threading.Lock()

    def _setup_radio(self):
        """Configura os pinos GPIO e inicializa a conexão serial."""
        if cfg.DEBUG_MODE:
            print("[LoRa] Configurando rádio e porta serial...")
            
        loractrl.set_mode("normal")
        
        try:
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
            print(f"[LoRa] Erro ao abrir porta serial ({cfg.PORT}): {e}")
            self.ser = None

        time.sleep(0.2)

    def _listen_loop(self):
        """Loop principal da thread que lê a serial continuamente."""
        if cfg.DEBUG_MODE:
            print("[LoRa Thread] Iniciando loop de recepção...")
            print(self.ser.port if self.ser else "Serial não inicializada.")
            print(self.ser.baudrate if self.ser else "Serial não inicializada.")
        
        buffer = b""
        while self.running:
            if cfg.DEBUG_MODE:
                print("[LoRa Thread] Aguardando dados...")
            try:
                if self.ser and self.ser.is_open and self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    buffer += data
                    if cfg.DEBUG_MODE:
                        print(f"[LoRa Thread] Dados recebidos: {data}")
                    
                    if b'\n' in buffer:
                        lines = buffer.split(b'\n')
                        msg_bruta = lines[-2].decode('utf-8', errors='ignore').strip()
                        buffer = lines[-1]
                        
                        if msg_bruta:
                            self.messages.put(msg_bruta)
                            if cfg.DEBUG_MODE:
                                print(
                                    f"📡 [LoRa Thread] Nova mensagem: {msg_bruta}"
                                )
                                print(
                                    f"📦 Fila: {self.messages.qsize()}"
                                )
                
                time.sleep(0.1)
            except Exception as e:
                if cfg.DEBUG_MODE:
                    print(f"[LoRa Error] Falha na leitura: {e}")
                time.sleep(1)

    def start(self):
        """Inicia a thread de recepção."""
        if not self.running:
            self._setup_radio()
            self.running = True
            self.thread = threading.Thread(target=self._listen_loop, daemon=True, name="LoRaListenThread")
            self.thread.start()
            if cfg.DEBUG_MODE:
                print("[LoRa] Thread de recepção iniciada em segundo plano.")

    def receive(self):
        """Retorna a próxima mensagem da fila."""
        if not self.messages.empty():
            return self.messages.get()
        return None

    def stop(self):
        """Finaliza a thread e fecha a serial."""
        if cfg.DEBUG_MODE:
            print("[LoRa] Encerrando recepção...")
        self.running = False
        if self.thread:
            self.thread.join(timeout=2)
        if self.ser and self.ser.is_open:
            self.ser.close()
        if cfg.DEBUG_MODE:
            print("[LoRa] Recursos liberados.")
