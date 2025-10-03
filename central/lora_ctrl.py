###########################################################################
# Controle do módulo LoRa E220-900M30S via Raspberry Pi
# Autor: Alvaro Adriano Beck
# Data: 2024-06-20
# 
# Requisitos:
# - Biblioteca pyserial: pip install pyserial
# - Biblioteca RPi.GPIO: pip install RPi.GPIO
# - Desabilitar bluetooth se estiver ativo (afeta /dev/serial0) - sudo raspi-config
#        editar o arquivo /boot/firmware/config.txt
#        adicionar: dtoverlay=pi3-disable-bt
# - Desabilitar a porta serial do Raspberry Pi (afeta /dev/serial0) - sudo raspi-config
#        Interfacing Options -> Serial -> No (login shell) -> Yes (enable serial port)
# - Conectar os pinos M0 e M1 do E220 aos GPIO17 e GPIO27 do Raspberry Pi respectivamente
# - Conectar o pino AUX do E220 ao GPIO25 do Raspberry Pi
# - Conectar TX do E220 ao RX do Raspberry Pi e RX do E220 ao TX do Raspberry Pi
# - Alimentar o E220 com 3.3V conforme especificação
# - Documentação do E220: https://www.waveshare.com/w/upload/0/0f/E220-900M30S_Datasheet_V1.3.pdf
###########################################################################
import config as cfg
import serial
import time
import RPi.GPIO as GPIO


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


def set_mode(mode: str):
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


def read_parameters(ser: serial.Serial) -> bytes | None:
    """Lê parâmetros atuais do E220.

    Args:
        ser: Instância serial.
    Returns:
        Parâmetros lidos (bytes) ou None em caso de falha.
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

def write_parameters_dynamic(ser: serial.Serial, **kwargs: any) -> bool:
    """
    Escreve parâmetros de forma dinâmica no módulo E220. Combinando com os valores padrão
    para os parâmetros não especificados.

    Args:
        ser (serial.Serial): Objeto da conexão serial.
        **kwargs: Parâmetros a serem atualizados (por exemplo, freq_mhz=915, power=30).
            freq_mhz (float): Frequência em MHz (850.125 a 929.575 para 900T30D).
            address (int): Endereço do dispositivo (0x0000 a 0xFFFF).
            power_dbm (int): Potência de transmissão em dBm (30, 27, 24, 21).
            speed (int): Velocidade de comunicação em bps (1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200).
            parity (str): Paridade ("8N1", "8O1", "8E1").
            air_data_rate (float): Air Data Rate em kbps (0.3, 1.2, 2.4, 4.8, 9.6, 19.2).
    Returns:
        bool: True se a escrita foi bem-sucedida, False caso contrário.
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

    # Cria uma cópia mutável dos parâmetros padrão
    current_params = bytearray(cfg.DEFAULT_PARAMS)
    
    if cfg.DEBUG_MODE:
        print(f"[E220] Parâmetros padrão: {current_params.hex()}")

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
        if power in power_table_w:
            current_params[4] = (current_params[4] & 0xFC) | power_table_w[power]
            print(f"Potência ajustada para {power} dBm")
        else:
            print(f"Aviso: Potência de {power} dBm não suportada.")

    if 'speed' in kwargs:
        speed = kwargs['speed']
        if speed in speed_table_w:
            # Mantém os outros bits de SPEED e atualiza apenas o baud rate
            current_params[2] = (current_params[2] & 0xF8) | speed_table_w[speed]
            print(f"Baud rate ajustado para {speed} bps")
        else:
            print(f"Aviso: Baud rate de {speed} bps não suportado.")

    if 'parity' in kwargs:
        parity = kwargs['parity']
        if parity in parity_table_w:
            current_params[2] = (current_params[2] & 0xE7) | (parity_table_w[parity] << 3)
            print(f"Paridade ajustada para {parity}")
        else:
            print(f"Aviso: Paridade '{parity}' não suportada.")

    if 'air_data_rate' in kwargs:
        air_rate = kwargs['air_data_rate']
        if air_rate in air_rate_table_w:
            current_params[2] = (current_params[2] & 0xF8) | air_rate_table_w[air_rate]
            print(f"Air Data Rate ajustado para {air_rate} kbps")
        else:
            print(f"Aviso: Air Data Rate de {air_rate} kbps não suportado.")

    if cfg.DEBUG_MODE:
        print(f"[E220] Parâmetros finais a serem escritos: {current_params.hex()}")

    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de escrita...")
    cmd = bytes([0xC0, 0x00, 0x08]) + current_params
    ser.write(cmd)
    time.sleep(2)

    if cfg.DEBUG_MODE:
        print("[E220] Enviando comando de leitura...")
    resp = ser.read(ser.in_waiting)
    
    if cfg.DEBUG_MODE:
        print(f"Resposta bruta: {resp.hex()}")
    
    set_mode("normal")

    # A resposta de sucesso para escrita é 11 bytes: 0xC1 0x00 0x08 + 8 bytes de dados
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

# central/lora_ctrl.py