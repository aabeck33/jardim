###########################################################################
# Configurações do Projeto - Controle central do sistema
# Módulos Raspberry Pi e LoRa E220-900M30S
# Autor: Alvaro Adriano Beck
# Data: 2024-06-20
# Versão: 2.0 (Arquitetura Modular)
###########################################################################

from pathlib import Path

from dotenv import dotenv_values

ENV_FILE = Path(__file__).resolve().parents[1] / ".env"
ENV_VALUES = dotenv_values(ENV_FILE)

DEBUG_MODE = True  # Modo de debug (True/False)

# Valores: teste (simulado), teste_radio (E220 real sem bomba) ou producao.
CENTRAL_MODE = (ENV_VALUES.get("CENTRAL_AMBIENTE") or "teste").strip().lower()
if CENTRAL_MODE not in {"teste", "teste_radio", "producao"}:
	raise ValueError(
		"CENTRAL_AMBIENTE deve ser 'teste', 'teste_radio' ou 'producao'."
	)
TEST_MESSAGE_INTERVAL = 5

STORMGLASS_API_KEY = (ENV_VALUES.get("STORMGLASS_API_KEY") or "").strip()
if CENTRAL_MODE == "producao" and not STORMGLASS_API_KEY:
	raise ValueError("Defina STORMGLASS_API_KEY em central/.env para produção.")

# Pinos de controle do E220
PIN_M0 = 17  # GPIO17 (pino físico 11)
PIN_M1 = 27  # GPIO27 (pino físico 13)
PIN_AUX = 25  # GPIO25 (pino físico 7)

# GPIO da bomba no Raspberry Pi
PIN_BOMBA = 18  # GPIO18 (pino físico 12)

# API de clima (Stormglass)
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
