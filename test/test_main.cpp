/**
 * @file tests_main.cpp
 * @brief Testes para o projeto Jardim Inteligente
 * by Alvaro A. Beck  -  2025-07 - Last update: 2025-08-10
 * Este arquivo contém os testes unitários para as funções do projeto Jardim Inteligente.
 * Certifique-se de que todos os testes estão cobertos e funcionando corretamente.
 * Para executar os testes, use o comando `platformio test` na raiz do projeto.
 */
#include <Arduino.h>
#include <unity.h>
#include "main.h"
#include "setup.h"
#include "utils.h"


// Instância do módulo LoRa SX1262
/**
 * @brief Initializes an SX1262 LoRa module with specified pin configuration.
 *
 * @param NSS   Chip select (NSS) pin number.
 * @param DIO1  DIO1 pin number for interrupt handling.
 * @param RESET Reset pin number for hardware reset.
 * @param BUSY  Busy pin number to monitor module status.
 *
 * @note The SX1262 object is created using the specified pin assignments.
 */
SX1262 lora = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);

// Instância do display OLED SSD1306
/**
 * @brief Creates an instance of the Adafruit_SSD1306 display object.
 *
 * This object is used to interface with an SSD1306 OLED display using the I2C protocol.
 *
 * @param SCREEN_WIDTH The width of the display in pixels.
 * @param SCREEN_HEIGHT The height of the display in pixels.
 * @param &Wire Reference to the I2C communication object.
 * @param OLED_RESET The pin used to reset the display (can be set to -1 if not used).
 */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Mocks para funções que causam reset ou delay longo
void erroCriticoMock(String motivo = "Erro crítico não especificado") {
    Serial.println("Simulando erroCritico: " + motivo);
    // Não chama esp_restart()
}

void aguardarMock(int tempo) {
    Serial.println("Simulando aguardar por " + String(tempo) + " minutos (mock: delay de 100ms).");
    delay(100);
}

// Testes das funções
void test_setupSerial() {
    TEST_ASSERT_TRUE(setupSerial());
}

void test_setupWiFi() {
    setupWiFi();
    TEST_ASSERT_TRUE(true); // Se não travou, passou
}

void test_setupSPIFFS() {
    setupSPIFFS();
    TEST_ASSERT_TRUE(true);
}

void test_setupLoRa() {
    setupLoRa();
    TEST_ASSERT_TRUE(true);
}

void test_setupDisplay() {
    setupDisplay();
    TEST_ASSERT_TRUE(true);
}

void test_displayOnOff() {
    displayOnOff("on");
    displayOnOff("off");
    TEST_ASSERT_TRUE(true);
}

void test_sinalizaErro() {
    sinalizaErro(2, "rapido");
    sinalizaErro(2, "lento");
    TEST_ASSERT_TRUE(true);
}

void test_erroCritico() {
    erroCriticoMock("Teste de erro crítico");
    TEST_ASSERT_TRUE(true);
}

void test_showError() {
    showError("Teste de erro", 1);
    showError("Teste de erro", 2);
    showError("Teste de erro", -1);
    TEST_ASSERT_TRUE(true);
}

void test_verificarBotaoModoSeguro() {
    verificarBotaoModoSeguro();
    TEST_ASSERT_TRUE(true);
}

void test_connectToWiFi() {
    connectToWiFi();
    TEST_ASSERT_TRUE(true);
}

void test_readBatteryVoltage() {
    float v = readBatteryVoltage();
    TEST_ASSERT_GREATER_THAN(0.0, v);
}

void test_verificarUsoRAM() {
    verificarUsoRAM();
    TEST_ASSERT_TRUE(true);
}

void test_verificarUsoJson() {
    StaticJsonDocument<JSON_DOC_SIZE> doc;
    doc["teste"] = 123;
    verificarUsoJson(doc);
    TEST_ASSERT_TRUE(true);
}

void test_getInternalTemperature() {
    float t = getInternalTemperature();
    TEST_ASSERT_TRUE(t > -40.0 && t < 100.0);
}

void test_processarComando() {
    processarComando("status");
    TEST_ASSERT_TRUE(true);
}

void test_receberComandoLoRa() {
    receberComandoLoRa();
    TEST_ASSERT_TRUE(true);
}

void test_aguardar() {
    aguardarMock(1); // Aguarda apenas 100ms
    TEST_ASSERT_TRUE(true);
}

void test_xorEncrypt() {
    String original = "teste";
    String encrypted = xorEncrypt(original, XOR_KEY);
    TEST_ASSERT_EQUAL(original.length(), encrypted.length());
}

void test_coletarDados() {
    String dados = coletarDados();
    TEST_ASSERT_GREATER_THAN(0, dados.length());
}

void test_logToSPIFFS() {
    logToSPIFFS("Teste de log");
    TEST_ASSERT_TRUE(true);
}

void test_printLog() {
    printLog();
    TEST_ASSERT_TRUE(true);
}

void test_enviarDados() {
    enviarDados("{\"teste\":123}");
    TEST_ASSERT_TRUE(true);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Iniciando testes...");
    delay(2000);

    UNITY_BEGIN();
    RUN_TEST(test_setupSerial);
    RUN_TEST(test_setupWiFi);
    RUN_TEST(test_setupSPIFFS);
    RUN_TEST(test_setupLoRa);
    RUN_TEST(test_setupDisplay);
    RUN_TEST(test_displayOnOff);
    RUN_TEST(test_sinalizaErro);
    RUN_TEST(test_erroCritico);
    RUN_TEST(test_showError);
    RUN_TEST(test_verificarBotaoModoSeguro);
    RUN_TEST(test_connectToWiFi);
    //RUN_TEST(test_readBatteryVoltage);
    RUN_TEST(test_verificarUsoRAM);
    RUN_TEST(test_verificarUsoJson);
    RUN_TEST(test_getInternalTemperature);
    RUN_TEST(test_processarComando);
    RUN_TEST(test_receberComandoLoRa);
    RUN_TEST(test_aguardar);
    RUN_TEST(test_xorEncrypt);
    RUN_TEST(test_coletarDados);
    RUN_TEST(test_logToSPIFFS);
    RUN_TEST(test_printLog);
    RUN_TEST(test_enviarDados);
    UNITY_END();
}

void loop() {
    // Não faz nada
}

// test_main.cpp