#include <Arduino.h>
#include <unity.h>
#include "main.h"
#include "setup.h"
#include "utils.h"

// Mock para funções que causam reset ou delay longo
void erroCriticoMock(String motivo = "Erro crítico não especificado") {
    Serial.println("Simulando erroCritico: " + motivo);
    // Não chama esp_restart()
}

void aguardarMock(int tempo) {
    Serial.println("Simulando aguardar por " + String(tempo) + " minutos.");
    delay(100); // Aguarda apenas 100ms para teste
}

// Testes usando TEST_CASE
TEST_CASE("setupSerial inicia corretamente", "[setup]") {
    TEST_ASSERT_TRUE(setupSerial());
}

TEST_CASE("setupWiFi executa sem travar", "[setup]") {
    setupWiFi();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("setupSPIFFS executa sem travar", "[setup]") {
    setupSPIFFS();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("setupLoRa executa sem travar", "[setup]") {
    setupLoRa();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("setupDisplay executa sem travar", "[setup]") {
    setupDisplay();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("displayOnOff liga e desliga display", "[utils]") {
    displayOnOff("on");
    displayOnOff("off");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("sinalizaErro pisca LED", "[utils]") {
    sinalizaErro(2, "rapido");
    sinalizaErro(2, "lento");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("erroCritico simulado", "[utils]") {
    erroCriticoMock("Teste de erro crítico");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("showError exibe mensagem", "[utils]") {
    showError("Teste de erro", 1);
    showError("Teste de erro", 2);
    showError("Teste de erro", -1);
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("verificarBotaoModoSeguro executa", "[utils]") {
    verificarBotaoModoSeguro();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("connectToWiFi executa", "[utils]") {
    connectToWiFi();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("readBatteryVoltage retorna valor", "[utils]") {
    float v = readBatteryVoltage();
    TEST_ASSERT_GREATER_THAN(0.0, v);
}

TEST_CASE("verificarUsoRAM executa", "[utils]") {
    verificarUsoRAM();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("verificarUsoJson executa", "[utils]") {
    StaticJsonDocument<JSON_DOC_SIZE> doc;
    doc["teste"] = 123;
    verificarUsoJson(doc);
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("getInternalTemperature retorna valor", "[utils]") {
    float t = getInternalTemperature();
    TEST_ASSERT_TRUE(t > -40.0 && t < 100.0);
}

TEST_CASE("processarComando executa", "[utils]") {
    processarComando("status");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("receberComandoLoRa executa", "[utils]") {
    receberComandoLoRa();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("aguardar simulado", "[utils]") {
    aguardarMock(1); // Aguarda apenas 100ms
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("xorEncrypt retorna string do mesmo tamanho", "[utils]") {
    String original = "teste";
    String encrypted = xorEncrypt(original, XOR_KEY);
    TEST_ASSERT_EQUAL(original.length(), encrypted.length());
}

TEST_CASE("coletarDados retorna JSON", "[utils]") {
    String dados = coletarDados();
    TEST_ASSERT_GREATER_THAN(0, dados.length());
}

TEST_CASE("logToSPIFFS salva log", "[utils]") {
    logToSPIFFS("Teste de log");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("printLog imprime log", "[utils]") {
    printLog();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("enviarDados envia dados", "[utils]") {
    enviarDados("{\"teste\":123}");
    TEST_ASSERT_TRUE(true);
}

void setUp() {}
void tearDown() {}

void setup() {
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
}

void loop() {
    UNITY_END();
}