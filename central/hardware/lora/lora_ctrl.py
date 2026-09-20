###########################################################################
# Controle do módulo LoRa E220-900M30S via Raspberry Pi
# Autor: Alvaro Adriano Beck
# Versão: 2.0 (Arquitetura Modular)
###########################################################################
from config import settings as cfg
import serial
import time
from typing import Any

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


power_table_r = {
    0b00: 30,   # Padrão
    0b01: 27,
    0b10: 24,
    0b11: 21
}
power_table_w = {v: k for k, v in power_table_r.items()}

speed_table_r = {
    0b000: 1200,
    0b001: 2400,
    0b010: 4800,
    0b011: 9600,  # Padrão
    0b100: 19200,
    0b101: 38400,
    0b110: 57600,
    0b111: 115200,
}
speed_table_w = {v: k for k, v in speed_table_r.items()}

air_rate_table_r = {
    0b000: 0.3,
    0b001: 1.2,
    0b010: 2.4,
    0b011: 4.8,
    0b100: 9.6,
    0b101: 19.2
}
air_rate_table_w = {v: k for k, v in air_rate_table_r.items()}

parity_table_r = {
    0b00: "8N1",
    0b01: "8O1",
    0b10: "8E1"
}
parity_table_w = {v: k for k, v in parity_table_r.items()}


def set_mode(mode: str = "normal"):
    """Define o modo do módulo E220 (M0, M1)."""
    if mode == "normal":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo NORMAL")
        GPIO.output(cfg.PIN_M0, 0)
        GPIO.output(cfg.PIN_M1, 0)
    elif mode == "config":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo CONFIGURAÇÃO/Sleep")
        GPIO.output(cfg.PIN_M0, 1)
        GPIO.output(cfg.PIN_M1, 1)
    elif mode == "wor-tx":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo Wake-on-Radio - Transmissão (WOR-TX)")
        GPIO.output(cfg.PIN_M0, 0)
        GPIO.output(cfg.PIN_M1, 1)
    elif mode == "wor-rx":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo Wake-on-Radio - Recepção (WOR-RX)")
        GPIO.output(cfg.PIN_M0, 1)
        GPIO.output(cfg.PIN_M1, 0)
    time.sleep(0.5)


def read_parameters(ser: serial.Serial) -> bytes | None:
    """Lê parâmetros atuais do E220."""
    cmd = bytes([0xC1, 0x00, 0x09])
    
    set_mode("config")

    if cfg.DEBUG_MODE:
        print("[E220] Limpando buffers...")
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    if cfg.DEBUG_MODE:
        print("[E220] Aguardando pino AUX...")
    while GPIO.input(cfg.PIN_AUX) == GPIO.LOW:
        time.sleep(0.01)
    
    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de leitura...")
    ser.write(cmd)
    time.sleep(0.1)
    resp = ser.read(ser.in_waiting)

    if cfg.DEBUG_MODE:
        print(f"Resposta bruta: {resp.hex()}")

    set_mode("normal")

    if len(resp) == 12 and resp[0:3] == b'\xc1\x00\x09':
        params = resp[3:]
        addh, addl, speed, option, chan = params[0], params[1], params[2], params[3], params[4]
        address = (addh << 8) | addl
        chan = chan & 0x7F
        freq = 850.125 + chan

        config = {
            'address': address,
            'baud_rate': speed_table_r[(speed >> 5) & 0b111],
            'parity': parity_table_r[(speed >> 3) & 0b11],
            'air_data_rate': air_rate_table_r[speed & 0b111],
            'channel': chan,
            'frequency_mhz': freq,
            'tx_power': power_table_r[option & 0b11]
        }

        print("\n--- [E220] Configurações Atuais do Módulo ---")
        print(f" Endereço: {config['address']:04X}")
        print(f" Baud Rate (UART): {config['baud_rate']} bps")
        print(f" Paridade: {config['parity']}")
        print(f" Air Data Rate: {config['air_data_rate']} kbps")
        print(f" Canal: {config['channel']}")
        print(f" Frequência: {config['frequency_mhz']} MHz")
        print(f" Potência TX: {config['tx_power']} dBm")
        print("-------------------------------------------\n")

        return resp
    else:
        print("[E220] Falha ao ler parâmetros")
        return None


def write_parameters(ser: serial.Serial, params: bytearray) -> bool:
    """Escreve novos parâmetros no E220."""
    if len(params) != 8:
        raise ValueError("Parâmetros devem ser um bytearray de 8 bytes.")
    elif cfg.DEBUG_MODE:
        print(f"[E220] Parâmetros a serem escritos: {params.hex()}")
    
    set_mode("config")

    if cfg.DEBUG_MODE:
        print("[E220] Limpando buffers...")
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    if cfg.DEBUG_MODE:
        print("[E220] Aguardando pino AUX...")
    while GPIO.input(cfg.PIN_AUX) == GPIO.LOW:
        time.sleep(0.01)

    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de escrita...")
    cmd = bytes([0xC0, 0x00, 0x08]) + params

    ser.write(cmd)
    time.sleep(0.2)

    resp = ser.read(ser.in_waiting)
    if cfg.DEBUG_MODE:
        print(f"Resposta bruta: {resp.hex()}")

    set_mode("normal")

    if len(resp) == 11 and resp[0] == 0xC1:
        print("[E220] Parâmetros escritos com sucesso")
        return True
    else:
        print("[E220] Falha ao escrever parâmetros")
        return False


def write_parameters_dynamic(ser: serial.Serial, **kwargs: Any) -> bool:
    """Escreve parâmetros de forma dinâmica no módulo E220."""
    set_mode("config")

    if cfg.DEBUG_MODE:
        print("[E220] Limpando buffers...")
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    if cfg.DEBUG_MODE:
        print("[E220] Aguardando pino AUX...")
    while GPIO.input(cfg.PIN_AUX) == GPIO.LOW:
        time.sleep(0.01)

    current_params = bytearray(cfg.DEFAULT_PARAMS)
    
    if cfg.DEBUG_MODE:
        print(f"[E220] Parâmetros padrão: {current_params.hex()}")

    if 'freq_mhz' in kwargs:
        freq_mhz = kwargs['freq_mhz']
        chan = int(freq_mhz - 850.125)
        current_params[3] = chan
        print(f"Canal ajustado para {chan} (Freq: {850.125 + chan} MHz)")

    if 'address' in kwargs:
        addr = kwargs['address']
        current_params[0] = (addr >> 8) & 0xFF
        current_params[1] = addr & 0xFF
        print(f"Endereço ajustado para {addr:04X}")

    if 'power_dbm' in kwargs:
        power = kwargs['power_dbm']
        if power in power_table_w:
            current_params[4] = (current_params[4] & 0xFC) | power_table_w[power]
            print(f"Potência ajustada para {power} dBm")

    if 'speed' in kwargs:
        speed = kwargs['speed']
        if speed in speed_table_w:
            current_params[2] = (current_params[2] & 0xF8) | speed_table_w[speed]
            print(f"Baud rate ajustado para {speed} bps")

    if 'parity' in kwargs:
        parity = kwargs['parity']
        if parity in parity_table_w:
            current_params[2] = (current_params[2] & 0xE7) | (parity_table_w[parity] << 3)
            print(f"Paridade ajustada para {parity}")

    if 'air_data_rate' in kwargs:
        air_rate = kwargs['air_data_rate']
        if air_rate in air_rate_table_w:
            current_params[2] = (current_params[2] & 0xF8) | air_rate_table_w[air_rate]
            print(f"Air Data Rate ajustado para {air_rate} kbps")

    if cfg.DEBUG_MODE:
        print(f"[E220] Parâmetros finais a serem escritos: {current_params.hex()}")
        print("[E220] Enviando comando de escrita...")

    cmd = bytes([0xC0, 0x00, 0x08]) + current_params
    ser.write(cmd)
    time.sleep(2)

    resp = ser.read(ser.in_waiting)
    
    if cfg.DEBUG_MODE:
        print(f"Resposta bruta: {resp.hex()}")
    
    set_mode("normal")

    if len(resp) == 11 and resp[0] == 0xC1:
        print("[E220] Parâmetros escritos com sucesso!")
        return True
    else:
        print(f"[E220] Falha ao escrever parâmetros. Resposta inesperada: {resp.hex()}")
        return False


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
        time.sleep(2)

        write_parameters(ser, cfg.DEFAULT_PARAMS)
        read_parameters(ser)

    except KeyboardInterrupt:
        print("Encerrando...")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        GPIO.cleanup()
