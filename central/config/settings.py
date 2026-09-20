###########################################################################
# Configurações do Projeto - Controle central do sistema
# Módulos Raspberry Pi e LoRa E220-900M30S
# Autor: Alvaro Adriano Beck
# Data: 2024-06-20
# Versão: 2.0 (Arquitetura Modular)
###########################################################################

DEBUG_MODE = True  # Modo de debug (True/False)

# Pinos de controle do E220
PIN_M0 = 17  # GPIO17 (pino físico 11)
PIN_M1 = 27  # GPIO27 (pino físico 13)
PIN_AUX = 25  # GPIO25 (pino físico 7)

# GPIO da bomba no Raspberry Pi
PIN_BOMBA = 18  # GPIO18 (pino físico 12)

# API de clima (Stormglass)
STORMGLASS_API_KEY = "7fe1931e-839f-11f0-b41a-0242ac130006-7fe193a0-839f-11f0-b41a-0242ac130006"
LATITUDE = -22.94348412102589
LONGITUDE = -47.03536345569916
STORMGLASS_URL = "https://api.stormglass.io/v2/weather/point"

# Configuração da porta serial /dev/serial0 ou /dev/ttyAMA0
PORT = "/dev/serial0"
BAUDRATE = 9600  # Velocidade de comunicação

# Configurações do módulo LoRa E220-900T30D
LORA_FREQ = 915     # Frequência em MHz
LORA_ADDRH = 0xFF   # Endereço do dispositivo (0x00 a 0xFF) - 0xFF = broadcast
LORA_ADDRL = 0xFF   # Endereço do dispositivo (0x00 a 0xFF) - 0xFF = broadcast
LORA_CHANNEL = 0x41 # Canal (0x00 a 0x50)
LORA_SPEED = 0x62   # Velocidade
LORA_WOR = 0x03     # Modo WOR
LORA_POWER = 0x00   # Potência

DEFAULT_PARAMS = bytearray([LORA_ADDRH, LORA_ADDRL, LORA_SPEED, LORA_POWER, LORA_CHANNEL, LORA_WOR, 0x00, 0x00])
