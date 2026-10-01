#ifndef JARDIM_BOARD_PINS_H
#define JARDIM_BOARD_PINS_H

#include <stddef.h>
#include <stdint.h>

constexpr uint8_t VBAT_READ = 1;
constexpr uint8_t LED_PIN = 35;
constexpr uint8_t PINO_VEXT = 36;
constexpr uint8_t SCREEN_ADDRESS = 0x3C;
constexpr uint8_t PINO_BOTAO_SAIR_SEGURO = 33;
constexpr uint8_t ANALOG_RESOLUTION = 12;
constexpr int pinosEntrada[] = {2, 3, 4, 5, 6, 7};
constexpr size_t numEntradas = sizeof(pinosEntrada) / sizeof(pinosEntrada[0]);

#if USE_DISPLAY
  #define OLED_SDA 17
  #define OLED_SCL 18
  #define OLED_RESET 21                         // 21 ou -1 para Reset por software
  constexpr uint8_t SCREEN_WIDTH = 128;
  constexpr uint8_t SCREEN_HEIGHT = 64;
#endif

#if USE_LORA
  #define LORA_NSS 8
  #define LORA_DIO1 14
  #define LORA_RST 12
  #define LORA_BUSY 13
  #define LORA_SCK 9
  #define LORA_MISO 11
  #define LORA_MOSI 10
#endif

#if USE_SERIAL_2
  #define SERIAL2_RX_PIN 41
  #define SERIAL2_TX_PIN 42
#endif

#if USE_LORA_EXT
  #define LORA_ADDRH 0xFF
  #define LORA_ADDRL 0xFF
  #define LORA_CHANNEL 0x41
  #define LORA_EXT_AUX 38
  #define LORA_EXT_M0 39
  #define LORA_EXT_M1 40
#endif

constexpr float freqLoRa = 915.125;
constexpr int txPower = 17;
constexpr int8_t sfLoRa = 11;
constexpr float bwLoRa = 125.0;
constexpr uint8_t crLoRa = 5;
constexpr uint16_t plLoRa = 8;
constexpr uint16_t swLoRa = 0x12;
constexpr uint8_t crcLoRa = 0;

#endif
