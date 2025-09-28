###########################################################################
# Controle do módulo LoRa E220-900M30S via Raspberry Pi
# Autor: Alvaro Adriano Beck
# Data: 2024-06-20
# 
# Requisitos:
# - Biblioteca pyserial: pip install pyserial
# - Biblioteca RPi.GPIO: pip install RPi.GPIO
# - Configurar a serial do Raspberry Pi (raspi-config)
# - Desabilitar bluetooth se estiver ativo (afeta /dev/serial0) - sudo raspi-config
#        editar o arquivo /boot/firmware/config.txt
#        adicionar: dtoverlay=pi3-disable-bt
# - Conectar os pinos M0 e M1 do E220 aos GPIO17 e GPIO27 do Raspberry Pi
# - Conectar TX do E220 ao RX do Raspberry Pi e RX do E220 ao TX do Raspberry Pi
# - Alimentar o E220 com 3.3V conforme especificação
# - Documentação do E220: https://www.waveshare.com/w/upload/0/0f/E220-900M30S_Datasheet_V1.3.pdf
###########################################################################
import config as cfg
import serial
import time
import RPi.GPIO as GPIO


# --- Mapeamento de Parâmetros ---
# Este dicionário traduz o valor do baud rate para o texto correspondente
BAUD_TABLE = {
    0b000: 1200,
    0b001: 2400,
    0b010: 4800,
    0b011: 9600,
    0b100: 19200,
    0b101: 38400,
    0b110: 57600,
    0b111: 115200,
}

# Este dicionário traduz o valor da air data rate para o texto correspondente
AIR_RATE_TABLE = {
    0b000: "0.3k",
    0b001: "1.2k",
    0b010: "2.4k",
    0b011: "4.8k",
    0b100: "9.6k",
    0b101: "19.2k",
    0b110: "38.4k",
    0b111: "62.5k",
}

# Este dicionário traduz o valor da potência para o texto correspondente
POWER_MAPr = {
    0b00: "30dBm",  # 30dBm
    0b01: "27dBm",  # 27dBm
    0b10: "24dBm",  # 24dBm
    0b11: "21dBm",  # 21dBm
}


# --- Mapeamento de Parâmetros ---
# Este dicionário traduz a potência em dBm para o byte de configuração
POWER_MAP = {
    30: 0x00,  # 30dBm
    27: 0x01,  # 27dBm
    24: 0x02,  # 24dBm
    21: 0x03,  # 21dBm
}

# Este dicionário traduz o baud rate para o byte de configuração
SPEED_MAP = {
    1200: 0x00,
    2400: 0x01,
    4800: 0x02,
    9600: 0x03,  # Padrão
    19200: 0x04,
    38400: 0x05,
    57600: 0x06,
    115200: 0x07,
}

def set_mode(mode):
    """Define o modo do módulo E220 (M0, M1).

    Args:
        mode (str): O modo a ser definido ("normal", "config", "wakeup", "power_saving").
    """
    if mode == "normal":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo NORMAL")
        GPIO.output(cfg.PIN_M0, 0)
        GPIO.output(cfg.PIN_M1, 0)
    elif mode == "config":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo CONFIGURAÇÃO/Sleep")
        GPIO.output(cfg.PIN_M0, 1)
        GPIO.output(cfg.PIN_M1, 1)
    elif mode == "wakeup":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo Wake-on-Radio - Transmissão (WOR-TX)")
        GPIO.output(cfg.PIN_M0, 0)
        GPIO.output(cfg.PIN_M1, 1)
    elif mode == "power_saving":
        if cfg.DEBUG_MODE: print("[E220] Entrando em modo Wake-on-Radio - Recepção (WOR-RX)")
        GPIO.output(cfg.PIN_M0, 1)
        GPIO.output(cfg.PIN_M1, 0)
    time.sleep(0.5)


def read_parameters(ser):
    """Lê parâmetros atuais do E220.

    PParameters:
        ser: Instância serial com os parâmetros lidos ou None em caso de falha.
    Returns:
        Parâmetros lidos ou None em caso de falha.
    """
    set_mode("config")

    if cfg.DEBUG_MODE:
        print("[E220] Limpando buffers...")
    ser.flushInput() # Limpa o buffer de entrada
    ser.flushOutput() # Limpa o buffer de saída

    if cfg.DEBUG_MODE:
        print("[E220] Aguardando pino AUX...")
    while GPIO.input(cfg.PIN_AUX) == GPIO.LOW:
        time.sleep(0.01)
    
    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de leitura...")
    cmd = bytes([0xC1, 0x00, 0x09]) # Comando de leitura (9 bytes de dados à partir do endereço 0x00)
    ser.write(cmd)
    time.sleep(0.1)
    resp = ser.read(ser.in_waiting)

    if cfg.DEBUG_MODE:
        print(f"Resposta bruta: {resp.hex()}")

    set_mode("normal")

    # A resposta de sucesso para leitura é 12 bytes: 0xC1 0x00 0x09 + 8 bytes de dados + 1 byte extra 0x10
    if len(resp) == 12 and resp[0:3] == b'\xc1\x00\x09':
        params = resp[3:]
        addh, addl, speed, option, chan, crypt_h, crypt_l = params[0], params[1], params[2], params[3], params[4], params[6], params[7]

        # Endereço do dispositivo 2 bytes à partir da posição 0
        address = (addh << 8) | addl
        # Baud rate 3 bits (5-7) a partir da posição 2
        # Air data rate 3 bits (0-2) a partir da posição 2
        # Paridade 2 bits (3-4) a partir da posição 2
        # Canal 7 bits a partir da posição 3
        chan = chan & 0x7F
        # Frequência aproximada em MHz = 850.125 + canal
        freq = 850.125 + chan
        # Potência TX 2 bits (6-7) a partir da posição 4

        config = {
            'address': address,
            'baud_rate': baud_from_speed(speed),
            'parity': parity_from_speed(speed),
            'air_data_rate': air_rate_from_speed(speed),
            'channel': chan,
            'frequency_mhz': freq,
            'tx_power': tx_power_from_option(option)
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


def write_parameters(ser, params):
    """Escreve novos parâmetros no E220 (permanente ou temporário).

    Args:
        ser (serial.Serial): Instância serial.
        params (bytearray): Bytearray com os 8 bytes de parâmetros a serem escritos.
    Returns:
        bool: True se a escrita foi bem-sucedida, False caso contrário.
    """
    if len(params) != 8:
        raise ValueError("Parâmetros devem ser um bytearray de 8 bytes.")
    else:
        if cfg.DEBUG_MODE:
            print(f"[E220] Parâmetros a serem escritos: {params.hex()}")
    
    set_mode("config")

    if cfg.DEBUG_MODE:
        print("[E220] Limpando buffers...")
    ser.flushInput() # Limpa o buffer de entrada
    ser.flushOutput() # Limpa o buffer de saída

    if cfg.DEBUG_MODE:
        print("[E220] Aguardando pino AUX...")
    while GPIO.input(cfg.PIN_AUX) == GPIO.LOW:
        time.sleep(0.01)

    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de escrita...")
    cmd = bytes([0xC0, 0x00, 0x08]) + params

    ser.write(cmd)
    time.sleep(0.2)

    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de leitura...")
    resp = ser.read(ser.in_waiting)
    if cfg.DEBUG_MODE:
        print(f"Resposta bruta: {resp.hex()}")

    set_mode("normal")

    # A resposta de sucesso para escrita é 11 bytes: 0xC1 0x00 0x08 + 8 bytes de dados
    if len(resp) == 11 and resp[0] == 0xC1:
        print("[E220] Parâmetros escritos com sucesso")
        return True
    else:
        print("[E220] Falha ao escrever parâmetros")
        return False


def baud_from_speed(speed_byte):
    baud_table = {
        0b000: 1200, 0b001: 2400, 0b010: 4800, 0b011: 9600,
        0b100: 19200, 0b101: 38400, 0b110: 57600, 0b111: 115200
    }
    return baud_table[(speed_byte >> 5) & 0b111]


def parity_from_speed(speed_byte):
    parity_table = {
        0b00: "8N1",  # 8 bits, No parity, 1 stop bit
        0b01: "8O1",  # 8 bits, Odd parity, 1 stop bit
        0b10: "8E1",  # 8 bits, Even parity, 1 stop bit
        0b11: "8N1"   # 8 bits, No parity, 1 stop bits (igual 00)
    }
    return parity_table[(speed_byte >> 3) & 0b11]


def air_rate_from_speed(speed_byte):
    air_table = {
        0b000: 0.3, 0b001: 1.2, 0b010: 2.4, 0b011: 4.8,
        0b100: 9.6, 0b101: 19.2
    }
    return air_table[speed_byte & 0b111]


def tx_power_from_option(option_byte):
    power_table = {0b00: 30, 0b01: 27, 0b10: 24, 0b11: 21}
    return power_table[option_byte & 0b11]


def write_parameters_dynamic(ser, **kwargs):
    """
    Escreve parâmetros de forma dinâmica no módulo E220.

    Args:
        ser (serial.Serial): Objeto da conexão serial.
        **kwargs: Parâmetros a serem atualizados (por exemplo, freq_mhz=915, power=30).
    """
    ser.flushInput() # Limpa o buffer de entrada
    ser.flushOutput() # Limpa o buffer de saída

    # Aguarda o pino AUX ficar HIGH
    while GPIO.input(cfg.PIN_AUX) == GPIO.LOW:
        time.sleep(0.01)

    # Cria uma cópia mutável dos parâmetros padrão
    current_params = bytearray(cfg.DEFAULT_PARAMS)
    
    # Processa os argumentos dinâmicos
    if 'freq_mhz' in kwargs:
        # Calcula o canal a partir da frequência (para 900T30D)
        freq_mhz = kwargs['freq_mhz']
        chan = int(freq_mhz - 850.125)
        current_params[3] = chan
        print(f"Canal ajustado para {chan} (Freq: {850.125 + chan} MHz)")

    if 'address' in kwargs:
        addr = kwargs['address']
        current_params[0] = (addr >> 8) & 0xFF  # ADDH
        current_params[1] = addr & 0xFF        # ADDL
        print(f"Endereço ajustado para {addr:04X}")

    if 'power_dbm' in kwargs:
        power = kwargs['power_dbm']
        if power in POWER_MAP:
            current_params[4] = (current_params[4] & 0xFC) | POWER_MAP[power]
            print(f"Potência ajustada para {power} dBm")
        else:
            print(f"Aviso: Potência de {power} dBm não suportada.")

    if 'speed' in kwargs:
        speed = kwargs['speed']
        if speed in SPEED_MAP:
            # Mantém os outros bits de SPEED e atualiza apenas o baud rate
            current_params[2] = (current_params[2] & 0xF8) | SPEED_MAP[speed]
            print(f"Baud rate ajustado para {speed} bps")
        else:
            print(f"Aviso: Baud rate de {speed} bps não suportado.")

    # Monta o comando final de escrita
    cmd = bytes([0xC0, 0x00, 0x08]) + current_params
    ser.write(cmd)

    # Aguarda a resposta
    time.sleep(2)
    resp = ser.read(ser.in_waiting)
    
    if len(resp) == 8 and resp[0] == 0xC1:
        print("[E220] Parâmetros escritos com sucesso!")
        return resp
    else:
        print(f"[E220] Falha ao escrever parâmetros. Resposta inesperada: {resp.hex()}")
        return None


if __name__ == "__main__":
    try:
        # Configuração da GPIO
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(cfg.PIN_M0, GPIO.OUT)
        GPIO.setup(cfg.PIN_M1, GPIO.OUT)
        GPIO.setup(cfg.PIN_AUX, GPIO.IN)

        # Porta serial do Raspberry (Lembrar de desativar o bluetooth se estiver usando /dev/serial0)
        ser = serial.Serial(cfg.PORT, baudrate=cfg.BAUDRATE, timeout=1, bytesize=8, parity='N', stopbits=1)

        print("Lendo configurações do módulo...")
        read_parameters(ser)

        # Voltar para modo normal
        set_mode("normal")

    except KeyboardInterrupt:
        pass
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        GPIO.cleanup()
