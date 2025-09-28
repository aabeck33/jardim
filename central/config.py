###########################################################################
# Configurações do PRojeto - COntrole central do sistema
# Módulos Raspberry Pi e LoRa E220-900M30S
# Autor: Alvaro Adriano Beck
# Data: 2024-06-20
# Versão: 1.0
###########################################################################

DEBUG_MODE = True  # Modo de debug (True/False)

# Pinos de controle do E220 - Necessário apenas para configurar o módulo
PIN_M0 = 17  # GPIO17
PIN_M1 = 27  # GPIO27
PIN_AUX = 25  # GPIO25 (pino AUX)

# GPIO da bomba no Raspberry Pi
PIN_BOMBA = 18

# API de clima
# https://docs.stormglass.io/?utm_campaign=website&utm_medium=email&utm_source=sendgrid#/
STORMGLASS_API_KEY = "7fe1931e-839f-11f0-b41a-0242ac130006-7fe193a0-839f-11f0-b41a-0242ac130006"
LATITUDE = -22.94348412102589
LONGITUDE = -47.03536345569916
STORMGLASS_URL = "https://api.stormglass.io/v2/weather/point"

# Configuração da porta serial /dev/serial0 ou /dev/ttyAMA0 - deve ser habilitada no raspi-config
PORT = "/dev/serial0"
BAUDRATE = 9600
# LoRa
LORA_FREQ = 915     # Frequência em MHz
LORA_ADDRH = 0xAA   # Endereço do dispositivo (0x00 a 0xFF)
LORA_ADDRL = 0xB1   # Endereço do dispositivo (0x00 a 0xFF)
LORA_CHANNEL = 0x41 # Canal (0x00 a 0x50 - 0-80 = 81 canais)
LORA_SPEED = 0x62   # Velocidade (0x00 a 0xFF) - 0x62 = 9600bps 8N1 | 2.4K
LORA_WOR = 0x03     # Modo WOR (0x00 a 0xFF) - 0x00 = 500ms, 0x03 = 1500ms, 0x07 = 4000ms
LORA_POWER = 0x00   # Potência (0x00 a 0x03) - 0x00 = 30dBm, 0x03 = 21dBm

# Parâmetros padrão do E220-900T30D
# ADDH, ADDL, SPEED (REG0), OPTION (REG1), CHANNEL (REG2), WOR (REG3), CRYPT_H, CRYPT_L
# Default: ([0xAA, 0xB1, 0x62, 0x00, 0x12, 0x03, 0x00, 0x00])
DEFAULT_PARAMS = bytearray([LORA_ADDRH, LORA_ADDRL, LORA_SPEED, LORA_POWER, LORA_CHANNEL, LORA_WOR, 0x00, 0x00])