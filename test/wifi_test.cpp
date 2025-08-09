#include <Arduino.h>
#include <unity.h>
#include <WiFi.h>

void test_wifi_status_should_be_disconnected_initially() {
  TEST_ASSERT_EQUAL(WL_DISCONNECTED, WiFi.status());
}

void setup() {
  // Inicializa o teste
  UNITY_BEGIN();
  RUN_TEST(test_wifi_status_should_be_disconnected_initially);
  UNITY_END();
}

void loop() {
  // Não é necessário fazer nada aqui
}
