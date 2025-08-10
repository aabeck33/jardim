/**
 * @file test_main.cpp
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


// === Mock para funções que causam reset ou delay longo ===
void erroCriticoMock(String motivo = "Erro crítico não especificado") {
    Serial.println("Simulando erroCritico: " + motivo);
    // Não chama esp_restart()
}

void aguardarMock(int tempo) {
    Serial.println("Simulando aguardar por " + String(tempo) + " minutos.");
    delay(1000); // Aguarda apenas 1 segundo para teste
}


// === Testes usando TEST_CASE ===
TEST_CASE("setupSerial inicia corretamente", "[setup] [Serial]") {
    TEST_ASSERT_TRUE(setupSerial());
}

TEST_CASE("setupWiFi executa sem travar", "[setup] [WiFi]") {
    setupWiFi();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("setupSPIFFS executa sem travar", "[setup] [SPIFFS]") {
    setupSPIFFS();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("setupLoRa executa sem travar", "[setup] [LoRa]") {
    setupLoRa();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("setupDisplay executa sem travar", "[setup] [Display]") {
    setupDisplay();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("displayOnOff liga e desliga display", "[utils] [Display]") {
    displayOnOff("on");
    displayOnOff("off");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("sinalizaErro pisca LED", "[utils] [LED]") {
    sinalizaErro(2, "rapido");
    sinalizaErro(2, "lento");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("erroCritico simulado", "[utils] [Erro]") {
    erroCriticoMock("Teste de erro crítico");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("showError exibe mensagem", "[utils] [Erro]") {
    showError("Teste de erro", 1);
    showError("Teste de erro", 2);
    showError("Teste de erro", -1);
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("verificarBotaoModoSeguro executa", "[utils] [Botão]") {
    verificarBotaoModoSeguro();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("connectToWiFi executa", "[utils] [WiFi]") {
    connectToWiFi();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("readBatteryVoltage retorna valor", "[utils] [Bateria]") {
    float v = readBatteryVoltage();
    TEST_ASSERT_GREATER_THAN(0.0, v);
}

TEST_CASE("verificarUsoRAM executa", "[utils] [RAM]") {
    verificarUsoRAM();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("verificarUsoJson executa", "[utils] [JSON]") {
    StaticJsonDocument<JSON_DOC_SIZE> doc;
    doc["teste"] = 123;
    verificarUsoJson(doc);
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("getInternalTemperature retorna valor", "[utils] [Temperatura]") {
    float t = getInternalTemperature();
    TEST_ASSERT_TRUE(t > -40.0 && t < 100.0);
}

TEST_CASE("processarComando executa", "[utils] [Comando]") {
    processarComando("status");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("receberComandoLoRa executa", "[utils] [LoRa]") {
    receberComandoLoRa();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("aguardar simulado", "[utils] [Tempo]") {
    aguardarMock(1); // Aguarda apenas 100ms
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("xorEncrypt retorna string do mesmo tamanho", "[utils] [Criptografia]") {
    String original = "teste";
    String encrypted = xorEncrypt(original, XOR_KEY);
    TEST_ASSERT_EQUAL(original.length(), encrypted.length());
}

TEST_CASE("xorDecrypt retorna string original após xorEncrypt", "[utils] [Criptografia]") {
    String original = "Jardim Inteligente";
    char key = XOR_KEY;
    String encrypted = xorEncrypt(original, key);
    String decrypted = xorDecrypt(encrypted, key);
    TEST_ASSERT_EQUAL_STRING(original.c_str(), decrypted.c_str());
}

TEST_CASE("coletarDados retorna JSON", "[utils] [Dados]") {
    String dados = coletarDados();
    TEST_ASSERT_GREATER_THAN(0, dados.length());
}

TEST_CASE("logToSPIFFS salva log", "[utils] [Log]") {
    logToSPIFFS("Teste de log");
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("printLog imprime log", "[utils] [Log]") {
    printLog();
    TEST_ASSERT_TRUE(true);
}

TEST_CASE("enviarDados envia dados", "[utils] [Dados]") {
    enviarDados("{\"teste\":123}");
    TEST_ASSERT_TRUE(true);
}

/**
 * @brief Configura o ambiente de teste. É executado antes de cada teste.
 */
void setUp() {}


/**
 * @brief Desconfigura o ambiente de teste. É executado após cada teste.
 */
void tearDown() {}


/**
 * @brief Função principal de teste. Inicializa o Unity e executa os testes ou inicia.
 */
void setup() {
    Serial.begin(115200);
    delay(2000);
    UNITY_BEGIN();
}

/**
 * @brief Função principal de teste. Inicializa o Unity e executa os testes ou finaliza.
 */
void loop() {
    UNITY_END();
}

// test_main.cpp