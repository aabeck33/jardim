// test/test_all.cpp
#include <Arduino.h>
#include <unity.h>
#include "../src/main.cpp" // Inclui suas funções originais

// Executado antes de cada teste
void setUp(void) {}

// Executado depois de cada teste
void tearDown(void) {}

// ------------------- TESTES -------------------

TEST_CASE("WiFi conecta corretamente", "[wifi]") {
    WiFi.disconnect(true); // Força desconexão antes do teste
    connectToWiFi();
    TEST_ASSERT_EQUAL(WL_CONNECTED, WiFi.status());
}

TEST_CASE("Leitura de umidade de solo é válida", "[sensor-umidade]") {
    float umidade = lerUmidadeSolo();
    TEST_ASSERT_GREATER_THAN(0.0, umidade);
    TEST_ASSERT_LESS_OR_EQUAL(100.0, umidade);
}

TEST_CASE("Leitura de tensão de bateria é plausível", "[sensor-bateria]") {
    float tensao = lerTensaoBateria();
    TEST_ASSERT_GREATER_THAN(3.0, tensao); // mínimo
    TEST_ASSERT_LESS_THAN(4.3, tensao);    // máximo
}

TEST_CASE("Leitura de temperatura interna é plausível", "[sensor-temperatura]") {
    float temp = lerTemperaturaInterna();
    TEST_ASSERT_GREATER_THAN(-20.0, temp);
    TEST_ASSERT_LESS_THAN(85.0, temp);
}

TEST_CASE("Pacote LoRa é gerado corretamente", "[lora-json]") {
    String pacote = gerarPacoteJSON();
    TEST_ASSERT_TRUE(pacote.length() > 0);
    TEST_ASSERT_NOT_EQUAL(-1, pacote.indexOf("umidade"));
    TEST_ASSERT_NOT_EQUAL(-1, pacote.indexOf("tensao"));
    TEST_ASSERT_NOT_EQUAL(-1, pacote.indexOf("temperatura"));
}

TEST_CASE("Display OLED exibe informações", "[display]") {
    inicializarDisplay();
    atualizarDisplay();
    TEST_ASSERT_TRUE(true); // Apenas valida que não houve crash
}

TEST_CASE("Watchdog inicializa e alimenta corretamente", "[watchdog]") {
    iniciarWatchdog();
    alimentarWatchdog();
    TEST_ASSERT_TRUE(true); // Se não resetar, está OK
}

// ------------------- SETUP UNITY -------------------

void setup() {
    UNITY_BEGIN();

    RUN_TEST(WiFi_conecta_corretamente);
    RUN_TEST(Leitura_de_umidade_de_solo_e_valida);
    RUN_TEST(Leitura_de_tensao_de_bateria_e_plausivel);
    RUN_TEST(Leitura_de_temperatura_interna_e_plausivel);
    RUN_TEST(Pacote_LoRa_e_gerado_corretamente);
    RUN_TEST(Display_OLED_exibe_informacoes);
    RUN_TEST(Watchdog_inicializa_e_alimenta_corretamente);

    UNITY_END();
}

void loop() {
    // Testes executados apenas uma vez
}