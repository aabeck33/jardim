#ifndef SETUP_H
#define SETUP_H
/** 
 * @file setup.h
 * @brief Configurações de inicialização do sistema e dispositivos.
 */
#include "utils.h"


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

    dispmsg("AP: " + String(ssid));
    dispmsg("IP: " + IP.toString(), 1);
  } else if (WIFI_MODE == WIFI_STA) {
    dispmsg("Conectando ao Wi-Fi...");

    // Configura o Wi-Fi como cliente
    WiFi.setSleep(false);           // Ativa/Desativa o modo de sono do Wi-Fi
    WiFi.setAutoReconnect(true);    // Habilita reconexão automática
    WiFi.setAutoConnect(true);      // Habilita conexão automática
    WiFi.setHostname(DISPOSITIVO);  // Define o hostname do dispositivo
    WiFi.begin(ssid, password);     // Conecta como cliente
    connectToWiFi();                // Faz a conexão se for estação
  } else {
    showError("Modo Wi-Fi inválido.", 1);
    sinalizaErro(ERRO_WIFI_PISCA, "rapido");
    delay(2000);
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
    dispmsg("SPIFFS sucesso.");
  }
}


/**
 * @brief Inicializa e configura o módulo LoRa.
 * Define frequência, potência, fator de espalhamento, largura de banda, taxa de codificação, preâmbulo e palavra de sincronização.
 * Sinaliza erro e entra em loop caso não consiga inicializar.
 * 
 * Fórmula aproximada do air bit rate:
 * Use isso para estimar a taxa (bits/s):
 *    Símbolos por segundo (Rs) = BW / 2^SF
 *    Bits por segundo (Rb) ≈ Rs * SF * (4/(4+CR))
 * Exemplos:
 *    SF7, BW=125 kHz, CR=4/5 → Rs = 125000/128 ≈ 976.56 sps
 *    Rb ≈ 976.56 * 7 * 0.8 ≈ 5.47 kbps
 *    SF12, BW=125 kHz, CR=4/5 → Rs = 125000/4096 ≈ 30.52 sps
 *    Rb ≈ 30.52 * 12 * 0.8 ≈ 292 bps
 */
void setupLoRa() {
  dispmsg("Inicializando LoRa...");

  int status = lora.begin(
    freqLoRa,    // Frequência em MHz
    bwLoRa,      // Largura de banda em kHz
    sfLoRa,      // Fator de espalhamento
    crLoRa,      // Taxa de codificação
    swLoRa,      // Palavra de sincronização
    txPower,     // Potência de transmissão em dBm
    plLoRa       // Comprimento do preâmbulo em símbolos
    );
  delay(100);

  if (status == RADIOLIB_ERR_NONE) {
    // Configuração do LoRa
    if (lora.setCRC(crcLoRa) != RADIOLIB_ERR_NONE) {
      showError("Erro ao habilitar CRC LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    } else {
      dispmsg("LoRa ini sucesso.");
    }
  } else {
    showError(String(status), 2);
    while (true) {
      showError("LoRa não iniciado.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
      delay(2000);
    }
  }
  delay(2000);
}


/**
 * @brief Inicializa o display OLED.
 * Configura tamanho, cor e posição do texto.
 * Sinaliza erro e entra em loop caso não consiga inicializar.
 */
void setupDisplay() {
  // Liga o circuito Vext
  VextOnOff();

  // RESET do OLED
  resetOLED();

  // Inicia o barramento I2C
  Wire.begin(OLED_SDA, OLED_SCL);
  Serial.println("Barramento I2C iniciado.");

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    showError("Falha ao inicializar o display OLED.", 1);
    while (true) {
      showError("Display não iniciado.", 1);
      sinalizaErro(ERRODISPLAY_PISCA, "rapido");
      delay(2000);
    }
  }
  display.display();
  Serial.println("Display OLED iniciado.");
  delay(500);
}

void iniciarPinos() {
  // Pinos de entrada
  for (int i = 0; i < numEntradas; i++) {
    pinMode(pinosEntrada[i], INPUT);
  }

  // Pinos individuais:
  pinMode(VBAT_READ, INPUT);                       // Pino da bateria
  pinMode(PINO_BOTAO_SAIR_SEGURO, INPUT_PULLUP);   // Pino do botão de sair do modo seguro
  pinMode(LED_PIN, OUTPUT);                        // Pino do LED integrado
  pinMode(PINO_VEXT, OUTPUT);                      // Pino Vext
  pinMode(OLED_RESET, OUTPUT);                     // Pino de reset do OLED
  analogReadResolution(ANALOG_RESOLUTION);
  Serial.println("Pinos configurados.");
}


/**
 * @brief Configura a Serial com timeout.
 * @return [Boolean] true se a Serial foi iniciada corretamente, false caso contrário.
 */
bool setupSerial() {
  unsigned long startMillis = millis();

  Serial.begin(BAUD_RATE);

  while (!Serial && millis() - startMillis < SERIAL_TIMEOUT_MS) {
    delay(100);
  }

  // Inicializa pinos
  iniciarPinos();

  #if (USE_DISPLAY)
    setupDisplay();
  #else
    dispmsg("Display OLED desativado.");
  #endif

  if (!Serial) {
    showError("Serial não iniciada.", 1);
    sinalizaErro(ERROSERIAL_PISCA, "rapido");
    delay(2000);
    /*
    while (true) {
      sinalizaErro(ERROSERIAL_PISCA, "rapido");
      delay(2000);
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
      delay(2000);
    #endif
    return true;
  }
}

#endif
// setup.h