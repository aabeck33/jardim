import time
import config.settings as cfg

try:
    import RPi.GPIO as GPIO
    GPIO_AVAILABLE = True
except (ImportError, RuntimeError):
    GPIO_AVAILABLE = False
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


def init_lora_gpio():
    """Inicializa somente os pinos de controle do rádio LoRa E220."""

    if cfg.DEBUG_MODE:
        if GPIO_AVAILABLE:
            print("[GPIO] Configurando pinos do rádio LoRa...")
        else:
            print("[GPIO Simulação] Modo simulado.")

    GPIO.setmode(GPIO.BCM)
    GPIO.setup(cfg.PIN_M0, GPIO.OUT)
    GPIO.setup(cfg.PIN_M1, GPIO.OUT)
    GPIO.setup(cfg.PIN_AUX, GPIO.IN)
    GPIO.output(cfg.PIN_M0, GPIO.LOW)
    GPIO.output(cfg.PIN_M1, GPIO.LOW)


def init_gpio():
    """Inicializa os pinos GPIO do rádio e da bomba no Raspberry Pi."""
    init_lora_gpio()

    if cfg.DEBUG_MODE and GPIO_AVAILABLE:
        print("[GPIO] Configurando pino da bomba...")

    # Bomba
    GPIO.setup(cfg.PIN_BOMBA, GPIO.OUT)
    GPIO.output(cfg.PIN_BOMBA, GPIO.LOW)


def aciona_bomba(segundos: int):
    """Liga a bomba de irrigação pelo tempo especificado em segundos."""
    print(f"💧 Ligando bomba por {segundos} segundos...")
    GPIO.output(cfg.PIN_BOMBA, GPIO.HIGH)
    time.sleep(segundos)
    GPIO.output(cfg.PIN_BOMBA, GPIO.LOW)
    print("💧 Bomba desligada.")


def cleanup_gpio():
    """Libera os recursos do GPIO ao encerrar a aplicação."""
    if cfg.DEBUG_MODE:
        print("[GPIO] Limpando GPIO...")
    GPIO.cleanup()
