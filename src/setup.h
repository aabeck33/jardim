// setup.h
#ifndef SETUP_H
#define SETUP_H

#include "utils.h"


/**
 * @brief Configura a Serial com timeout.
 * @return true se a Serial foi iniciada corretamente, false caso contrário.
 */
bool setupSerial() {
  unsigned long startMillis = millis();

  Serial.begin(BAUD_RATE);

  while (!Serial && millis() - startMillis < SERIAL_TIMEOUT_MS) {
    delay(100);
  }

  if (!Serial) {
    showError("Serial não iniciada.", 1);
    sinalizaErro(ERROSERIAL_PISCA, "rapido");
    delay(3000);
    /*
    while (true) {
      sinalizaErro(ERROSERIAL_PISCA, "rapido");
      delay(3000);
    }
    */
    return false; // Retorna falso se a Serial não foi iniciada
  } else {
    Serial.println("Serial iniciada com sucesso.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("Serial iniciada.");
      display.display();
      delay(3000);
    #endif
    return true;
  }
}


/**
 * @brief Configura o Wi-Fi conforme o modo definido (AP ou STA).
 * Remove credenciais, define modo, configura AP ou inicia conexão como cliente.
 */
void setupWiFi() {
  WiFi.disconnect(true); // Remove credenciais
  delay(100);
  WiFi.mode(WIFI_MODE); // Define o modo

  if (WIFI_MODE == WIFI_AP) {
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();

    Serial.println("Ponto de acesso criado: " + String(ssid));
    Serial.println("IP Local: " + IP.toString());
    #if USE_DISPLAY
      displayOnOff();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("AP: ");
      display.print(ssid);
      display.setCursor(0, 8);
      display.print("IP: " + IP.toString());
      display.display();
      delay(3000);
    #endif
  } else {
    Serial.println("Conectando ao Wi-Fi...");
    #if USE_DISPLAY
      displayOnOff();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Conectando ao Wi-Fi...");
      display.display();
      delay(1000);
    #endif

    // Configura o Wi-Fi como cliente
    WiFi.setSleep(false);           // Ativa/Desativa o modo de sono do Wi-Fi
    WiFi.setAutoReconnect(true);    // Habilita reconexão automática
    WiFi.setAutoConnect(true);      // Habilita conexão automática
    WiFi.setHostname(DISPOSITIVO);  // Define o hostname do dispositivo
    WiFi.begin(ssid, password);     // Conecta como cliente
    //connectToWiFi();                // Faz a conexão se for estação
  }
}


/**
 * @brief Inicializa e monta o sistema de arquivos SPIFFS.
 * Exibe mensagem de sucesso ou sinaliza erro caso não consiga montar.
 */
void setupSPIFFS() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    showError("Erro ao montar SPIFFS", 1);
    sinalizaErro(ERROSPIFFS_PISCA, "rapido");
    while (true) {
      showError("SPIFFS não montado.", 1);
    }
  } else {
    Serial.println("SPIFFS montado com sucesso.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("SPIFFS sucesso!");
      display.display();
      delay(3000);
    #endif
  }
}


/**
 * @brief Inicializa e configura o módulo LoRa.
 * Define frequência, potência, fator de espalhamento, largura de banda, taxa de codificação, preâmbulo e palavra de sincronização.
 * Sinaliza erro e entra em loop caso não consiga inicializar.
 */
void setupLoRa() {
  Serial.println("Inicializando LoRa...");
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(6 * 0, 8 * 0);
    display.print("Inicializando LoRa...");
    display.display();
    delay(3000);
  #endif

  int status = lora.begin();

  if (status == RADIOLIB_ERR_NONE) {
    // Configuração do LoRa
    if (lora.setFrequency(freqLoRa) != RADIOLIB_ERR_NONE) { // Frequência em MHz (ajuste conforme sua região)
      showError("Erro ao definir frequência LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    if (lora.setOutputPower(txPower) != RADIOLIB_ERR_NONE) { // Potência de transmissão
      showError("Erro ao definir potência de transmissão LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    if (lora.setSpreadingFactor(7) != RADIOLIB_ERR_NONE) { // Fator de espalhamento
      showError("Erro ao definir fator de espalhamento LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    if (lora.setBandwidth(125.0) != RADIOLIB_ERR_NONE) { // Largura de banda
      showError("Erro ao definir largura de banda LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    if (lora.setCodingRate(5) != RADIOLIB_ERR_NONE) { // Taxa de codificação
      showError("Erro ao definir taxa de codificação LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    // Quantidade de redundância para correção de erros. Quanto maior, mais robusto, mas menos eficiente.
    // 5 equivale a 4/5. (Mais confiável = menor velocidade)
    if (lora.setPreambleLength(8) != RADIOLIB_ERR_NONE) { // Comprimento do preâmbulo (símbolos)
      showError("Erro ao definir comprimento do preâmbulo LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    if (lora.setSyncWord(0x12) != RADIOLIB_ERR_NONE) { // Palavra de sincronização. 0x34 → LoRaWAN público. | 0x12 → LoRa privado.
      showError("Erro ao definir palavra de sincronização LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    }
    Serial.println("LoRa inicializado com sucesso.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("LoRa iniciado com sucesso.");
      display.display();
    #endif
  } else {
    showError(String(status), 2);
    while (true) {
      showError("LoRa não iniciado.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
      delay(3000); // Aguarda 3 segundos.
    }
  }
  delay(3000);
}


/**
 * @brief Inicializa o display OLED.
 * Configura tamanho, cor e posição do texto.
 * Sinaliza erro e entra em loop caso não consiga inicializar.
 */
void setupDisplay() {
  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    showError("Falha ao inicializar o display OLED.", 1);
    while (true) {
      showError("Display não iniciado.", 1);
      sinalizaErro(ERRODISPLAY_PISCA, "rapido");
      delay(3000);
    }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(6 * 0, 8 * 0);
  Serial.println("Display OLED iniciado.");
  display.print("OLED iniciado.");
  display.display();
  delay(3000);
}

#endif
// setup.h
// ...existing code...