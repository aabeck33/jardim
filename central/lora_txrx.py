# Script para comunicação LoRa utilizando o módulo E220

import config as cfg
import lora_ctrl as loractrl
import serial
import time
import RPi.GPIO as GPIO


def xor_decrypt(input_str: str, key: int) -> str:
    return ''.join([chr(ord(c) ^ key) for c in input_str])


def send_message(ser: serial.Serial, msg: str):
    if cfg.DEBUG_MODE:
        print("Enviando mensagem...")
    ser.write(msg.encode("utf-8"))
    if cfg.DEBUG_MODE:
        print(f"[TX] {msg}")


def receive_message(ser: serial.Serial) -> str | None:
    espera: int = 0

    if cfg.DEBUG_MODE:
        print("Aguardando mensagem...")
    while not ser.in_waiting and espera < 10:
        time.sleep(0.5)
        espera += 1

    if ser.in_waiting:
        data = ser.read(ser.in_waiting)
        if cfg.DEBUG_MODE:
            print(f"[RX] {data.decode('utf-8', errors='ignore')}")
        return data.decode("utf-8", errors="ignore")
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
        time.sleep(2)

        loractrl.set_mode("NORMAL")
        time.sleep(0.5)

        while True:
            print("Enviando mensagem...")
            send_message(ser, "Hello LoRa E220!")
            time.sleep(2)
            print("Aguardando resposta...")
            receive_message(ser)
            time.sleep(2)

    except KeyboardInterrupt:
        print("Encerrando comunicação.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
        GPIO.cleanup()

# central/lora_txrx.py