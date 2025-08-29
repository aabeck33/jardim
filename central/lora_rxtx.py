import time
from SX127x.LoRa import *
from SX127x.board_config import BOARD

BOARD.setup()

class LoRaRcvCont(LoRa):
    def __init__(self, verbose=False):
        super(LoRaRcvCont, self).__init__(verbose)
        self.set_mode(MODE.SLEEP)
        self.set_pa_config(pa_select=1)
        self.set_freq(915.0)
        self.set_spreading_factor(7)
        self.set_bandwidth(BW.BW125)
        self.set_coding_rate(CODING_RATE.CR4_5)
        self.set_preamble(8)
        self.set_sync_word(0x12)
        self.set_rx_crc(True)
        self.set_power(14)

    def start(self):
        print("Iniciando LoRa Receiver...")
        self.reset_ptr_rx()
        self.set_mode(MODE.RXCONT)

    def receive(self):
        if self.get_irq_flags()["rx_done"]:
            self.clear_irq_flags(RxDone=1)
            payload = self.read_payload(nocheck=True)
            try:
                msg = ''.join([chr(c) for c in payload if c < 128])
                return msg
            except:
                return None
        return None

BOARD.teardown()
