# Script para comunicação LoRa utilizando o módulo E220
from config import settings as cfg
from . import lora_ctrl as loractrl
import serial
import time

try:
    import RPi.GPIO as GPIO
except (ImportError, RuntimeError):
    class DummyGPIO:
        BCM = "BCM"
        OUT = "OUT"
        IN = "IN"
        HIGH = 1
        LOW = 0
        @staticmethod
        def setmode(mode): pass
        @staticmethod
        def setup(pin, mode): pass
        @staticmethod
        def output(pin, value): pass
        @staticmethod
        def input(pin): return 1
        @staticmethod
        def cleanup(): pass
    GPIO = DummyGPIO()


def xor_decrypt(input_str: str, key: int) -> str:
    return ''.join([chr(ord(c) ^ key) for c in input_str])


def send_message(ser: serial.Serial, msg: str):
    if cfg.DEBUG_MODE:
        print("Enviando mensagem...")
    ser.write(msg.encode("utf-8"))
    if cfg.DEBUG_MODE:
        print(f"[TX] {msg}")


def receive_message(ser: serial.Serial, timeout: int = 10) -> str | None:
    start: float = time.time()
    data: bytes = b""

    if cfg.DEBUG_MODE:
        print("Aguardando mensagem...")
    while (time.time() - start) < timeout:
        if ser.in_waiting:
            data += ser.read(ser.in_waiting)
            if data.endswith(b'\n'):
                break
        time.sleep(0.05)

    if data:
        if cfg.DEBUG_MODE:
            print(f"[RX] {data.decode('utf-8', errors='ignore').strip()}")
        return data.decode("utf-8", errors="ignore").strip()
    else:
        if cfg.DEBUG_MODE:
            print("Nenhuma mensagem recebida.")
        return None


if __name__ == "__main__":
    try:
        if cfg.DEBUG_MODE:
            print("Configurando GPIO...")
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(cfg.PIN_M0, GPIO.OUT)
        GPIO.setup(cfg.PIN_M1, GPIO.OUT)
        GPIO.setup(cfg.PIN_AUX, GPIO.IN)
        time.sleep(0.5)

        if cfg.DEBUG_MODE:
            print("Iniciando LoRa Controller...")
        ser = serial.Serial(
            cfg.PORT, 
            baudrate=cfg.BAUDRATE, 
            timeout=1, 
            bytesize=8, 
            parity='N', 
            stopbits=1
        )
        if cfg.DEBUG_MODE:
            print("Serial aberta:", ser.port)
            print("Baudrate:", ser.baudrate)
        time.sleep(2)

        loractrl.write_parameters(ser, cfg.DEFAULT_PARAMS)
        time.sleep(0.5)
        
        loractrl.read_parameters(ser)
        time.sleep(0.5)

        while True:
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                print("Recebido:", data, data.hex())
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("Encerrando comunicação.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        GPIO.cleanup()
