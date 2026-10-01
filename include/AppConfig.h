#ifndef JARDIM_APP_CONFIG_H
#define JARDIM_APP_CONFIG_H

#ifndef DEBUG_MODE
  #define DEBUG_MODE true
#endif
#ifndef TEST_MODE
  #define TEST_MODE true
#endif
#ifndef USE_BATTERY
  #define USE_BATTERY false
#endif
#ifndef USE_DISPLAY
  #define USE_DISPLAY true
#endif
#ifndef USE_LORA
  #define USE_LORA false
#endif
#ifndef USE_LORA_EXT
  #define USE_LORA_EXT true
#endif
#ifndef USE_ENCRYPTION
  #define USE_ENCRYPTION false
#endif
#ifndef USE_DEEP_SLEEP
  #define USE_DEEP_SLEEP false
#endif
#ifndef USE_SPIFFS
  #define USE_SPIFFS false
#endif
#ifndef USE_WIFI
  #define USE_WIFI false
#endif
#ifndef USE_BLUETOOTH
  #define USE_BLUETOOTH false
#endif
#ifndef USE_SERIAL
  #define USE_SERIAL true
#endif
#ifndef USE_SERIAL_2
  #define USE_SERIAL_2 true
#endif
#ifndef RECEIVE_COMMANDS
  #define RECEIVE_COMMANDS true
#endif

constexpr const char* NOME_PROJETO = "Jardim_Horta Inteligente";
constexpr const char* DISPOSITIVO = "aabeck-01";
constexpr const char* TIPO_DISPOSITIVO = "ESP32V3";
constexpr const char* VERSAO_FIRMWARE = "0.0.3-alpha";
constexpr uint8_t PROTOCOLO_TELEMETRIA = 1;
constexpr size_t JSON_DOC_SIZE = 1024;
constexpr size_t JSON_USAGE_WARNING_PERCENT = 85;
constexpr size_t TEMPO_ENVIO = 11;
constexpr uint32_t INTERVALO_ENVIO_MS = TEMPO_ENVIO * 60000UL;
constexpr uint32_t INTERVALO_ENVIO_US = TEMPO_ENVIO * 60000000ULL;
constexpr uint8_t XOR_KEY = 0x5A;
constexpr uint32_t BAUD_RATE = 9600;
constexpr uint16_t SERIAL_TIMEOUT_MS = 5000;
constexpr uint32_t DEEP_SLEEP_TIMEOUT_MS = 60000;
constexpr uint32_t WDT_TIMEOUT_MS = 60000;
constexpr const char* SPIFFS_MOUNT_POINT = "/spiffs";
constexpr wifi_mode_t WIFI_MODE = WIFI_AP;
constexpr uint32_t INTERVALO_RECONEXAO_WIFI = 180000;
constexpr uint16_t WIFI_TIMEOUT = 10000;

#endif
