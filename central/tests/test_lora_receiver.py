import sys
import json
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.append(str(Path(__file__).resolve().parent.parent))

import config.settings as cfg
from hardware.gpio_controller import init_lora_gpio
from hardware.lora.lora_receiver import LoRaRcvCont


class LoRaReceiverTests(unittest.TestCase):
    def test_queues_all_complete_messages_and_keeps_partial_message(self):
        receiver = LoRaRcvCont(mode="producao")

        receiver._queue_complete_lines(b'{"id":1}\n{"id":')
        receiver._queue_complete_lines(b'2}\n')

        self.assertEqual(receiver.receive(timeout=0), '{"id":1}')
        self.assertEqual(receiver.receive(timeout=0), '{"id":2}')

    def test_empty_queue_returns_timeout_without_stopping_receiver(self):
        receiver = LoRaRcvCont(mode="teste")

        self.assertIsNone(receiver.receive(timeout=0))
        self.assertFalse(receiver.running)

    def test_rejects_unknown_mode(self):
        with self.assertRaises(ValueError):
            LoRaRcvCont(mode="staging")

    def test_radio_test_mode_opens_the_serial_radio(self):
        receiver = LoRaRcvCont(mode="teste_radio")

        with (
            patch("hardware.lora.lora_ctrl.set_mode") as set_mode,
            patch("serial.Serial") as serial_open,
            patch("hardware.lora.lora_receiver.time.sleep"),
        ):
            receiver._setup_radio()

        set_mode.assert_called_once_with("normal")
        serial_open.assert_called_once()

    def test_lora_gpio_setup_does_not_configure_pump_pin(self):
        with patch("hardware.gpio_controller.GPIO") as gpio:
            init_lora_gpio()

        configured_pins = [call.args[0] for call in gpio.setup.call_args_list]
        self.assertEqual(
            configured_pins,
            [cfg.PIN_M0, cfg.PIN_M1, cfg.PIN_AUX],
        )

    def test_test_mode_emits_sensor_telemetry_without_hardware(self):
        receiver = LoRaRcvCont(mode="teste")
        receiver.start()
        try:
            payload = json.loads(receiver.receive(timeout=1))
        finally:
            receiver.stop()

        self.assertEqual(payload["dispositivo"], "esp32-teste-01")
        self.assertEqual(payload["sensores"][0]["tipo"], "umidade_solo")


if __name__ == "__main__":
    unittest.main()