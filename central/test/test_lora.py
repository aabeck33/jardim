import serial
import time

# Configuração da porta serial
ser = serial.Serial(
    port="/dev/ttyAMA0",  # /dev/serial0 ou /dev/ttyAMA0 dependendo do seu Pi
    baudrate=9600,        # igual ao configurado no E220
    timeout=1
)


def xor_decrypt(input_str, key):
    return ''.join([chr(ord(c) ^ key) for c in input_str])


def send_message(msg: str):
    ser.write(msg.encode("utf-8"))
    print(f"[TX] {msg}")

def receive_message():
    if ser.in_waiting:
        data = ser.read(ser.in_waiting)
        print(f"[RX] {data.decode('utf-8', errors='ignore')}")


if __name__ == "__main__":
    try:
        while True:
            print("Enviando mensagem...")
            send_message("Hello LoRa E220!")
            time.sleep(2)
            print("Aguardando resposta...")
            receive_message()
    except KeyboardInterrupt:
        print("Encerrando comunicação.")
        ser.close()