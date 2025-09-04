from SX127x.LoRa import *
from SX127x.board_config import BOARD
import time
import threading
import keyboard

BOARD.setup()


def xor_decrypt(input_str, key):
    return ''.join([chr(ord(c) ^ key) for c in input_str])


class LoRaReceiver(LoRa):
    def __init__(self, verbose=False):
        super(LoRaReceiver, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_dio_mapping([0, 0, 0, 0])

    def on_rx_done(self):
        self.clear_irq_flags(RxDone=1)

        payload = self.read_payload(nocheck=True)
        message = ''.join([chr(c) for c in payload])
        decrypted_message = xor_decrypt(message, key=0x42)

        print("Mensagem recebida:", decrypted_message)

        self.set_mode(MODE.SLEEP)
        self.set_mode(MODE.RXCONT)


class LoRaSender(LoRa):
    def __init__(self, verbose=False):
        super(LoRaSender, self).__init__(verbose)
        self.set_mode(MODE.STDBY)
        self.set_dio_mapping([1, 0, 0, 0])

    def send_message(self, message):
        self.write_payload([ord(c) for c in message])
        self.set_mode(MODE.TX)
        print("Mensagem enviada:", message)
        time.sleep(1)
        self.set_mode(MODE.STDBY)


def lora_setup(send_receive):
    if send_receive == "send":
        lora = LoRaSender(verbose=False)
    else:
        lora = LoRaReceiver(verbose=False)
    lora.set_freq(915)
    lora.set_pa_config(pa_select=1)
    lora.set_spreading_factor(7)
    lora.set_bw(7)
    lora.set_coding_rate(CODING_RATE.CR4_5)
    lora.set_preamble(8)
    lora.set_sync_word(0x12)
    return lora


def lora_listen(lora):
    lora.set_mode(MODE.RXCONT)
    print("Aguardando mensagens LoRa...")
    while True:
        if lora.get_irq_flags()['rx_done']:
            lora.on_rx_done()
        time.sleep(0.1)


def keyboard_listen():
    print("Pressione qualquer tecla para executar a função.")
    while True:
        event = keyboard.read_event()
        if event.event_type == keyboard.KEY_DOWN:
            print("Tecla pressionada! Executando função...")
            # Chame sua função aqui
            time.sleep(0.2)  # Evita múltiplos triggers


if __name__ == "__main__":
    lora = lora_setup("receive")

    def start_lora_thread():
        t = threading.Thread(target=lora_listen, args=(lora,), daemon=True)
        t.start()
        return t

    def start_thread(target_func, *args):
        t = threading.Thread(target=target_func, args=args, daemon=True)
        t.start()
        return t

    t1 = start_thread(lora_listen, lora)
    t2 = start_thread(keyboard_listen)

    try:
        while True:
            if not t1.is_alive():
                print("Thread LoRa caiu! Reiniciando...")
                t1 = start_thread(lora_listen, lora)
            if not t2.is_alive():
                print("Thread teclado caiu! Reiniciando...")
                t2 = start_thread(keyboard_listen)
            time.sleep(1)
    except KeyboardInterrupt:
        print("Encerrando programa.")
        BOARD.teardown()



'''
lora = lora_setup("send")
try:
    while True:
        lora.send_message("Hello LoRa!")
        time.sleep(5)
except KeyboardInterrupt:
    print("Encerrando transmissor LoRa")
    BOARD.teardown()

lora = lora_setup("receive")
lora.set_mode(MODE.RXCONT)
try:
    print("Aguardando mensagens LoRa...")
    while True:
        pass
except KeyboardInterrupt:
    print("Encerrando receptor LoRa")
    BOARD.teardown()
'''