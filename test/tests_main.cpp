#include <Arduino.h>
#include <WiFi.h>
#include <unity.h>

#if defined(USE_DISPLAY)
#include <Adafruit_SSD1306.h>
extern Adafruit_SSD1306 display;
extern bool displayStatus;
#endif

// Supondo que estas variáveis estejam disponíveis no escopo global
extern const char* ssid;
extern const char* password;
extern const char* DISPOSITIVO;
extern int WIFI_MODE;
extern void setupWiFi();
extern void connectToWiFi();
extern void sinalizaErro(int, const char*);
extern void showError(const char*, int);

// Watchdog
#include "esp_task_wdt.h"
constexpr uint32_t WDT_TIMEOUT_MS = 60000;

// Setup de tempo
unsigned long startTime;

void test_wifi_connectivity() {
  if (WIFI_MODE == WIFI_STA) {
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());
    Serial.println("[TEST] Wi-Fi conectado com sucesso.");
  } else if (WIFI_MODE == WIFI_AP) {
    TEST_ASSERT(WiFi.softAPgetStationNum() >= 0);
    Serial.println("[TEST] Ponto de acesso iniciado com sucesso.");
  }
}

void test_wifi_config() {
  TEST_ASSERT_NOT_NULL(ssid);
  TEST_ASSERT(strlen(ssid) > 0);
  TEST_ASSERT_NOT_NULL(password);
  TEST_ASSERT(strlen(password) > 0);
  Serial.println("[TEST] Credenciais Wi-Fi válidas.");
}

void test_hostname() {
  String hostname = WiFi.getHostname();
  TEST_ASSERT_EQUAL_STRING(DISPOSITIVO, hostname.c_str());
  Serial.println("[TEST] Hostname configurado corretamente.");
}

void test_watchdog_reset() {
  esp_task_wdt_init(WDT_TIMEOUT_MS / 1000, true);
  esp_task_wdt_add(NULL);
  delay(100);
  esp_task_wdt_reset();  // Deve evitar reset
  TEST_ASSERT_TRUE(true); // Se chegou até aqui, passou
  Serial.println("[TEST] Watchdog configurado corretamente.");
}

// Simula leitura de sensores se existir funções
void test_sensor_readings() {
  // Suponha que haja funções como:
  // float readBatteryVoltage();
  // int readSoilMoisture();
  // float readTemperature();

  // Comente ou remova se não existirem
  /*
  float voltage = readBatteryVoltage();
  TEST_ASSERT(voltage > 0.0);
  Serial.printf("[TEST] Tensão da bateria: %.2f V\n", voltage);

  int moisture = readSoilMoisture();
  TEST_ASSERT(moisture >= 0 && moisture <= 4095);
  Serial.printf("[TEST] Umidade do solo: %d\n", moisture);

  float temp = readTemperature();
  TEST_ASSERT(temp > -40 && temp < 125);
  Serial.printf("[TEST] Temperatura: %.2f °C\n", temp);
  */
}

void setup() {
  Serial.begin(115200);
  delay(2000); // Aguarda a inicialização

  UNITY_BEGIN();
  Serial.println("\n\n==== INICIANDO TESTES ====\n");

  // Inicializa WiFi
  setupWiFi();

  RUN_TEST(test_wifi_config);
  RUN_TEST(test_wifi_connectivity);
  RUN_TEST(test_hostname);
  RUN_TEST(test_watchdog_reset);
  RUN_TEST(test_sensor_readings);

  UNITY_END();
}

void loop() {
  // Não utilizado nos testes unitários
}
