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
#include <SPI.h>
#include <ArduinoJson.h>
#include <RadioLib.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <esp_task_wdt.h>
#include <SPIFFS.h>
#include <LoRa_E220.h>
#include "secrets.h"
#include <AppConfig.h>
#include <BoardPins.h>
#include <StatusCode.h>
#include <SystemContext.h>
#include <services/CommandProcessor.h>

extern Configuration configE220std; // Configuração padrão do LoRa Externo


constexpr const char* ssid = WIFI_SSID;
constexpr const char* password = WIFI_PASSWORD;

// Atribui valor persistente mesmo após deep sleep (mantido na RAM RTC)
extern RTC_DATA_ATTR SystemContext systemContext;

#if (USE_WIFI)
  extern unsigned long ultimaTentativaWiFi; // Armazena o tempo da última tentativa de conexão Wi-Fi
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
#if (USE_LORA)
extern SX1262 lora;
#endif
#if (USE_DISPLAY)
extern Adafruit_SSD1306 display;
#endif
#if (USE_LORA_EXT)
extern LoRa_E220 LoRaExt;
#endif


// === FUNÇÕES ===
// setup.h
bool setupSerial();
bool setupSerial2();
void setupWiFi();
void setupSPIFFS();
void setupLoRa();
bool setupLoRaExt();
void setupDisplay();
void iniciarPinos();
bool setupBluetooth();
bool setupLoRaExt();
bool setupLoRaExtOld();
void testeE220Bruto();


// utils.h
void displayOnOff(const String &state = "on");
void sinalizaErro(const uint8_t numPisca, const String &frequencia = "lento");
void showError(const String &message = "Erro não especificado.", const uint8_t errorType = -1);
void verificarBotaoModoSeguro();
void connectToWiFi();
float readBatteryVoltage();
void dispmsg(const String &msg, const uint8_t linha = 0, const uint8_t coluna = 0,
  const uint8_t tamanho = 1, const uint8_t corTexto = SSD1306_WHITE, 
  const uint8_t corFundo = SSD1306_BLACK, const bool inverter = false, const bool forceDelay = false);
void verificarUsoRAM();
void verificarUsoJson(const StaticJsonDocument<JSON_DOC_SIZE> &doc);
float getInternalTemperature(const String &unidade = "celsius");
void processarComando(const String &cmd);
StatusCode registrarComando(const char* name, CommandHandler handler);
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
void wait_aux_high();
void set_mode(const String &mode = "normal");
bool isNumber(const String &str);
void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
bool readParametersE220Bin();
bool writeParametersE220Bin();
boolean read_parameters();
bool write_parameters(const Configuration &config);
uint8_t* read_parametersBin(HardwareSerial &ser);
bool write_parametersBin(HardwareSerial &ser, uint8_t params[8]);


#endif
// main.h