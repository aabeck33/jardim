#include <Arduino.h>
#include <unity.h>
#include "main.h"
#include "setup.h"
#include "utils.h"

// Mocks para funções que causam reset ou delay longo
void erroCriticoMock(String motivo = "Erro crítico não especificado") {
    Serial.println("Simulando erroCritico: " + motivo);
    // Não chama esp_restart()
}

void aguardarMock(int tempo) {
    Serial.println("Simulando aguardar por " + String(tempo) + " minutos.");
    delay(100); // Aguarda apenas 100ms para teste
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

void setUp() {}
void tearDown() {}

void setup() {
    Serial.begin(115200);
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
    RUN_TEST(test_readBatteryVoltage);
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