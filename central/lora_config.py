import serial
import time

# ============================
# Configuração UART Raspberry Pi
# ============================
ser = serial.Serial(
    port="/dev/serial0",  # ou /dev/ttyAMA0 dependendo da config
    baudrate=9600,
    timeout=1
)

# ============================
# Função genérica para enviar comando AT
# ============================
def at_command(cmd: str, delay=0.2):
    if not ser.is_open:
        ser.open()
    ser.write((cmd + "\r\n").encode("utf-8"))
    time.sleep(delay)
    resp = ser.read_all().decode(errors="ignore").strip()
    print(f"[AT] {cmd} -> {resp}")
    return resp


# ============================
# Ler parâmetros atuais
# ============================
def e220_read_config():
    print("[E220] Entrando em modo de configuração para leitura...")
    print("⚠️ Coloque M0=1 e M1=1 antes de rodar este comando.")
    time.sleep(2)

    # Verifica comunicação
    at_command("AT")

    # Leitura de parâmetros
    at_command("AT+ADDR?")
    at_command("AT+NETID?")
    at_command("AT+FREQ?")
    at_command("AT+PARAM?")
    at_command("AT+RFPOWER?")
    at_command("AT+MODE?")

    print("[E220] Leitura finalizada.")
    print("⚠️ Volte M0=0 e M1=0 para operação normal.")


# ============================
# Configurar módulo E220
# ============================
def e220_config():
    print("[E220] Entrando em modo de configuração...")
    print("⚠️ Coloque M0=1 e M1=1 no módulo antes de continuar!")

    time.sleep(2)
    ser.flushInput()
    ser.flushOutput()

    # Verifica se responde
    at_command("AT")
    
    # Configuração básica (exemplo compatível ESP32)
    at_command("AT+ADDR=0001")         # endereço local
    at_command("AT+NETID=01")          # ID da rede
    at_command("AT+FREQ=915000000")    # frequência central 915 MHz
    at_command("AT+PARAM=9,7,1,7")     # UART 9600, SF7, BW=125kHz, CR=4/5
    at_command("AT+RFPOWER=30")        # potência máxima (30 dBm)
    at_command("AT+MODE=0")            # modo transparente
    
    # Salva config em flash
    at_command("AT+SAVE")

    print("[E220] Configuração aplicada com sucesso.")
    print("⚠️ Agora coloque M0=0 e M1=0 para voltar ao modo normal.")


# ============================
# Enviar mensagem
# ============================
def e220_send(msg: str):
    if not ser.is_open:
        ser.open()
    ser.write(msg.encode("utf-8"))
    print(f"[TX] Enviado: {msg}")

# ============================
# Receber mensagem
# ============================
def e220_receive():
    if not ser.is_open:
        ser.open()
    if ser.in_waiting > 0:
        data = ser.read(ser.in_waiting)
        msg = data.decode("utf-8", errors="ignore")
        print(f"[RX] Recebido: {msg}")
        return msg
    return None

# ============================
# Teste principal
# ============================
if __name__ == "__main__":
    modo = input("Digite 'config', 'read', 'tx' ou 'rx': ").strip()

    if modo == "config":
        e220_config()

    elif modo == "read":
        e220_read_config()

    elif modo == "tx":
        print("⚠️ Certifique-se de que M0=0 e M1=0 (modo normal).")
        try:
            while True:
                e220_send("Hello from Raspberry Pi + E220")
                time.sleep(5)
        except KeyboardInterrupt:
            print("\nEncerrando TX...")
            ser.close()

    elif modo == "rx":
        print("⚠️ Certifique-se de que M0=0 e M1=0 (modo normal).")
        print("Aguardando mensagens...")
        try:
            while True:
                e220_receive()
                time.sleep(0.2)
        except KeyboardInterrupt:
            print("\nEncerrando RX...")
            ser.close()

    else:
        print("Modo inválido.")
