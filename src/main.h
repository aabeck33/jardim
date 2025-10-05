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
 *   - Adafruit_GFX for OLED display
 *   - Wire for OLED display
 *   - esp_task_wdt for watchdog timer
 *   - SPIFFS for file system support
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <esp_task_wdt.h>
#include <SPIFFS.h>


// === Configurações do programa ===
#define DEBUG_MODE false        // Modo de depuração
#define USE_DISPLAY true        // Usar display OLED
#define USE_LORA true           // Usar LoRa para comunicação
#define USE_BATTERY false       // Usar monitoramento da bateria
#define USE_ENCRYPTION true     // Usar encriptação
#define USE_DEEP_SLEEP true    // Usar sono profundo para economia de energia
#define USE_SPIFFS false        // Usar SPIFFS para armazenamento de arquivos
#define USE_WIFI false          // Usar Wi-Fi para comunicação
#define USE_SERIAL true         // Usar Serial para depuração
#define RECEIVE_COMMANDS false  // Receber comandos via LoRa - Não usar com USE_DEEP_SLEEP

// Dispositivos internos
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RESET 21                          // 21 ou -1 para Reset por software
constexpr uint8_t SCREEN_ADDRESS = 0x3C;       // Endereço I2C do OLED
constexpr uint8_t VBAT_READ = 1;               // Pino analógico para monitoramento da bateria
#define LORA_NSS 8
#define LORA_DIO1 14
#define LORA_RST 12
#define LORA_BUSY 13
constexpr uint8_t LED_PIN = 35;                // Pino do LED embutido (GPIO 35)
constexpr uint8_t PINO_VEXT = 36;              // Pino para ligar o circuito Vext

// GPIO
constexpr uint8_t PINO_BOTAO_SAIR_SEGURO = 33;

// === Variáveis e Constantes Globais ===
// Identificação:
constexpr const char* NOME_PROJETO = "Jardim_Horta Inteligente";
constexpr const char* DISPOSITIVO = "aabeck-01";
constexpr const char* TIPO_DISPOSITIVO = "ESP32V3";
constexpr const char* VERSAO_FIRMWARE = "0.0.2-alpha"; // Versão do firmware
constexpr const char* ssid = "aabeck-ESP32";           // SSID do Wi-Fi
constexpr const char* password = "EbSePc3k2&";         // Senha do Wi-Fi

constexpr size_t JSON_DOC_SIZE = 512;                  // Tamanho alocado
constexpr size_t JSON_USAGE_WARNING_PERCENT = 85;      // Percentual de uso que aciona o alerta
constexpr size_t TEMPO_ENVIO = 10;                     // Tempo de envio em minutos
constexpr uint8_t XOR_KEY = 0x5A;                      // Chave de encriptação XOR simples
// Cada caractere no display ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
constexpr uint8_t SCREEN_WIDTH = 128;                  // Largura do OLED
constexpr uint8_t SCREEN_HEIGHT = 64;                  // Altura do OLED
constexpr uint32_t BAUD_RATE = 9600;                   // Taxa de transmissão da Serial
constexpr uint16_t SERIAL_TIMEOUT_MS = 5000;           // Timeout da Serial em milissegundos

// Um ADC (Conversor Analógico-Digital) de 12 bits gera valores de 0 a 4095 (2¹² - 1).
// Portanto, se sua tensão de referência for 3.3V, o valor 4095 representa 3.3V, e 0 representa 0V.
// Cada unidade no valor representa cerca de 0.0008V (3.3V ÷ 4096).
constexpr uint8_t ANALOG_RESOLUTION = 12;
// Pinos ADC: GPIO 2, 3, 4, 5, 6, 7, 19, 20
// Pinos somente digitais: GPIO 33,  34, 38, 39, 40, 42, 42, 45, 46, 47?, 48?
constexpr int pinosEntrada[] = {2, 3, 4, 5, 6, 7};
constexpr size_t numEntradas = sizeof(pinosEntrada) / sizeof(pinosEntrada[0]);

// Configuração do LoRa - Os valores aqui precisam estar de acordo com o módulo utilizado
// e com as regulamentações locais de frequência e potência.
// Além de isso, os parâmetros de modulação (SF, BW, CR) devem ser ajustados conforme a aplicação,
// considerando o trade-off entre alcance, taxa de dados e robustez da comunicação.
// Ainda precisam estar iguais nos dispositivos que irão se comunicar.
constexpr float freqLoRa = 915.125;    // Frequência em MHz - Banda ISM para América do Sul - 915 a 928 MHz
constexpr int txPower = 17;            // Potência de transmissão (em dBm) — limite ANATEL é 20 dBm. As opções comuns são 2 a 17 dBm.
constexpr int8_t sfLoRa = 10;          // Fator de espalhamento (7 a 12) - Quanto maior, mais alcance / menor taxa
constexpr float bwLoRa = 125.0;        // Largura de banda (em kHz) - Quanto maior, maior taxa / menor alcance. As opções comuns são 125.0, 250.0, 500.0
constexpr uint8_t crLoRa = 5;          // Taxa de codificação (5 a 8) - 5 equivale a 4/5. (Mais confiável = menor velocidade)
constexpr uint16_t plLoRa = 8;         // Comprimento do preâmbulo (símbolos) - Quanto maior, mais confiável / menor velocidade. As opções são 6, 8, 10, 12, 14, 16, 18, 20
constexpr uint16_t swLoRa = 0x12;      // Palavra de sincronização - 0x34 para LoRaWAN público | 0x12 para LoRa privado
constexpr uint8_t crcLoRa = 0;         // HDesabilitar verificação de CRC

// Configuração do Deep Sleep
constexpr uint32_t DEEP_SLEEP_TIMEOUT_MS = 60000; // Timeout do Deep Sleep em milissegundos (1 minuto)

// Configuração do watchdog
constexpr uint32_t WDT_TIMEOUT_MS = 60000;             // Timeout do watchdog em milissegundos (1 minuto)
// Configuração do SPIFFS
constexpr const char* SPIFFS_MOUNT_POINT = "/spiffs";  // Ponto de montagem do SPIFFS
// Configuração do Wi-Fi
constexpr wifi_mode_t WIFI_MODE = WIFI_AP;             // Modo Wi-Fi: WIFI_STA (cliente), WIFI_AP (ponto de acesso) ou WIFI_AP_STA (ambos)
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
void iniciarPinos();

// utils.h
void displayOnOff(const String &state = "on");
void sinalizaErro(const uint8_t numPisca, const String &frequencia = "lento");
void showError(const String &message = "Erro não especificado.", const uint8_t errorType = -1);
void verificarBotaoModoSeguro();
void connectToWiFi();
float readBatteryVoltage();
void dispmsg(const String &msg, const uint8_t linha = 0, const uint8_t coluna = 0,
  const uint8_t tamanho = 1, const uint8_t corTexto = SSD1306_WHITE, 
  const uint8_t corFundo = SSD1306_BLACK, const bool inverter = false);
void verificarUsoRAM();
void verificarUsoJson(const StaticJsonDocument<JSON_DOC_SIZE> &doc);
float getInternalTemperature(const String &unidade = "celsius");
void processarComando(const String &cmd);
void receberComandoLoRa();
void aguardar(const uint8_t tempo);
String xorEncrypt(const String &input, const char key);
String xorDecrypt(const String &input, const char key);
String coletarDados();
void logToSPIFFS(const String &message);
void printLog();
void enviarDados(const String &payload);
void VextOnOff(const String &state = "On");
void resetOLED();

#endif
// main.h