#ifndef MAIN_H
#define MAIN_H
/** 
 * @file main.h
 * @brief Biblioteca principal do projeto Jardim Inteligente
 * Libraries used:
 *   - Arduino for basic functions
 *   - ArduinoJson for JSON serialization
 *   - RadioLib for LoRa communication
 *   - WiFi for Wi-Fi connectivity
 *   - Adafruit SSD1306 for OLED display
 *   - esp_task_wdt for watchdog timer
 *   - SPIFFS for file system support
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>
#include <SPIFFS.h>


// === Configurações do programa ===
#define DEBUG_MODE false        // Modo de depuração
#define USE_DISPLAY true        // Usar display OLED
#define USE_LORA true           // Usar LoRa para comunicação
#define USE_BATTERY false       // Usar monitoramento da bateria
#define USE_ENCRYPTION true     // Usar encriptação
#define USE_DEEP_SLEEP false    // Usar sono profundo para economia de energia
#define USE_SPIFFS false        // Usar SPIFFS para armazenamento de arquivos
#define USE_WIFI false          // Usar Wi-Fi para comunicação
#define USE_SERIAL true         // Usar Serial para depuração
#define RECEIVE_COMMANDS false  // Receber comandos via LoRa - Não usar com USE_DEEP_SLEEP


// === Variáveis e Constantes Globais ===
constexpr const char* NOME_PROJETO = "Jardim_Horta Inteligente";
constexpr const char* DISPOSITIVO = "aabeck-01";
constexpr const char* TIPO_DISPOSITIVO = "ESP32V3";
constexpr const char* VERSAO_FIRMWARE = "0.0.2-alpha"; // Versão do firmware
constexpr const char* ssid = "aabeck-ESP32";           // SSID do Wi-Fi
constexpr const char* password = "EbSePc3k2&";         // Senha do Wi-Fi
constexpr int8_t OLED_RESET = -1;                      // Reset por software
constexpr size_t JSON_DOC_SIZE = 512;                  // Tamanho alocado
constexpr size_t JSON_USAGE_WARNING_PERCENT = 85;      // Percentual de uso que aciona o alerta
constexpr size_t TEMPO_ENVIO = 10;                     // Tempo de envio em minutos
constexpr uint8_t XOR_KEY = 0x5A;                      // Chave de encriptação XOR simples
constexpr uint8_t SCREEN_ADDRESS = 0x3C;               // Endereço I2C do OLED
// Cada caractere no display ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
constexpr uint8_t SCREEN_WIDTH = 128;                  // Largura do OLED
constexpr uint8_t SCREEN_HEIGHT = 64;                  // Altura do OLED
constexpr uint8_t BATTERY_PIN = 34;                    // Pino analógico para monitoramento da bateria
constexpr uint32_t BAUD_RATE = 115200;                 // Taxa de transmissão da Serial
constexpr uint16_t SERIAL_TIMEOUT_MS = 5000;           // Timeout da Serial em milissegundos
constexpr uint8_t LED_PIN = 35;                        // Pino do LED embutido (GPIO 35)
// Sensores de umidade do solo
constexpr int pinosUmidade[] = {25, 32, 33, 35, 36, 39};  // Pinos ADC disponíveis no ESP32
constexpr size_t numSensoresUmidade = sizeof(pinosUmidade) / sizeof(pinosUmidade[0]);
// Obs: Evite usar GPIOs 34 a 39 para saída digital, eles são apenas de entrada analógica.
constexpr uint8_t PINO_BOTAO_SAIR_SEGURO = 27;
// Configuração do LoRa
constexpr float freqLoRa = 915.0;   // Frequência em MHz - Banda ISM para América do Sul
constexpr int txPower = 14;         // Potência de transmissão (em dBm) — limite ANATEL é 20 dBm
// Configuração do watchdog
constexpr uint32_t WDT_TIMEOUT_MS = 60000;             // Timeout do watchdog em milissegundos (1 minuto)
// Configuração do SPIFFS
constexpr const char* SPIFFS_MOUNT_POINT = "/spiffs";  // Ponto de montagem do SPIFFS
// Configuração do Wi-Fi
constexpr wifi_mode_t WIFI_MODE = WIFI_STA;            // Modo Wi-Fi: WIFI_STA (cliente), WIFI_AP (ponto de acesso) ou WIFI_AP_STA (ambos)
constexpr uint32_t INTERVALO_RECONEXAO_WIFI = 180000;  // Intervalo de reconexão Wi-Fi em milissegundos
constexpr uint16_t WIFI_TIMEOUT = 10000;               // Timeout do Wi-Fi em milissegundos

// Atribui valor persistente mesmo após deep sleep (mantido na RAM RTC)
RTC_DATA_ATTR bool modoSeguro = false;   // Modo seguro para evitar loops infinitos
bool displayStatus = false;              // Status do display OLED
bool serialOk = false;                   // Indica se a Serial foi iniciada corretamente

#if (USE_WIFI)
  unsigned long ultimaTentativaWiFi = 0; // Armazena o tempo da última tentativa de conexão Wi-Fi
#endif


// === Lista de piscadas de LED ===
#define ERROCRIT_PISCA 10       // 10 piscadas rápidas
#define ERRODISPLAY_PISCA 3     // 3 piscadas rápidas
#define ERROLORA_PISCA 4        // 4 piscadas rápidas
#define ERROSPIFFS_PISCA 5      // 5 piscadas rápidas
#define ERROSERIAL_PISCA 6      // 6 piscadas rápidas
#define ERRO_WIFI_PISCA 7       // 7 piscadas rápidas
#define MODOSEGURO_PISCA 12     // 12 piscadas rápidas


// === OBJETOS GLOBAIS ===
extern SX1262 lora;
extern Adafruit_SSD1306 display;


// === FUNÇÕES ===
// setup.h
bool setupSerial();
void setupWiFi();
void setupSPIFFS();
void setupLoRa();
void setupDisplay();

// utils.h
void displayOnOff(String state = "on");
void sinalizaErro(uint8_t numPisca, String frequencia = "lento");
void erroCritico(String motivo = "Erro crítico não especificado");
void showError(String message, int8_t errorType = -1);
void verificarBotaoModoSeguro();
void connectToWiFi();
float readBatteryVoltage();
void verificarUsoRAM();
void verificarUsoJson(const StaticJsonDocument<JSON_DOC_SIZE> &doc);
float getInternalTemperature();
void processarComando(const String &cmd);
void receberComandoLoRa();
void aguardar(int tempo);
String xorEncrypt(const String &input, char key);
String xorDecrypt(const String &input, char key);
String coletarDados();
void logToSPIFFS(const String &message);
void printLog();
void enviarDados(const String &payload);

#endif
// main.h