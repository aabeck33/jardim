import threading
import time
import serial
from config import settings as cfg
from . import lora_ctrl as loractrl

class LoRaRcvCont:
    def __init__(self):
        """Inicializa a classe para recepção contínua de dados via LoRa (módulo E220)."""
        self.ser = None
        self.thread = None
        self.running = False
        self.last_message = None
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
        except Exception as e:
            print(f"[LoRa] Erro ao abrir porta serial ({cfg.PORT}): {e}")
            self.ser = None

        time.sleep(0.2)

    def _listen_loop(self):
        """Loop principal da thread que lê a serial continuamente."""
        buffer = b""
        while self.running:
            try:
                if self.ser and self.ser.is_open and self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    buffer += data
                    
                    if b'\n' in buffer:
                        lines = buffer.split(b'\n')
                        msg_bruta = lines[-2].decode('utf-8', errors='ignore').strip()
                        buffer = lines[-1]
                        
                        if msg_bruta:
                            with self.lock:
                                self.last_message = msg_bruta
                                if cfg.DEBUG_MODE:
                                    print(f"📡 [LoRa Thread] Nova mensagem: {msg_bruta}")
                
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
        """Retorna a última mensagem recebida e limpa o buffer interno."""
        with self.lock:
            msg = self.last_message
            self.last_message = None
            return msg

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
