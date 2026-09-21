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

    dispmsg("AP: " + String(ssid), 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
    dispmsg("IP: " + IP.toString(), 1, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
  } else if (WIFI_MODE == WIFI_STA) {
    dispmsg("Conectando ao Wi-Fi...", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);

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
  dispmsg("Inicializando SPIFFS...", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);

  if (!SPIFFS.begin(true)) {
    showError("Erro ao montar SPIFFS", 1);
    sinalizaErro(ERROSPIFFS_PISCA, "rapido");
    delay(2000);
    modoSeguro = true;
    esp_restart();
  } else {
    dispmsg("SPIFFS sucesso.", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
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
  dispmsg("Inicializando LoRa...", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);

  int state = lora.begin(
    freqLoRa,    // Frequência em MHz
    bwLoRa,      // Largura de banda em kHz
    sfLoRa,      // Fator de espalhamento
    crLoRa,      // Taxa de codificação
    swLoRa,      // Palavra de sincronização
    txPower,     // Potência de transmissão em dBm
    plLoRa,      // Comprimento do preâmbulo em símbolos
    1.6,         // float tcxoVoltage = (1.6F) - Tensão TCXO (0 para cristal)
    false        // Usar regulador LDO (true) ou DC-DC (false)
    );
  delay(100);

  #if (DEBUG_MODE)
    dispmsg("Estado LoRa: " + String(state), 1, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
  #endif

  if (state == RADIOLIB_ERR_NONE) {
    // Configuração do LoRa
    if (lora.setCRC(crcLoRa) != RADIOLIB_ERR_NONE) {
      showError("Erro ao habilitar CRC LoRa.", 1);
      sinalizaErro(ERROLORA_PISCA, "rapido");
    } else {
      dispmsg("LoRa ini sucesso.", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
    }
  } else {
    String errorMsg = String(state) + " - LoRa não iniciado.";
    showError(errorMsg, 2);
    sinalizaErro(ERROLORA_PISCA, "rapido");
    delay(2000);
    modoSeguro = true;
    esp_restart();
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
    sinalizaErro(ERRODISPLAY_PISCA, "rapido");
    delay(2000);
    modoSeguro = true;
    esp_restart();
  }
  display.display();
  Serial.println("Display OLED iniciado.");
  delay(500);
}

void iniciarPinos() {
  #if (USE_LORA)
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);
    Serial.println("SPI iniciado para LoRa.");
  #endif

  // Pinos de entrada
  for (int i = 0; i < numEntradas; i++) {
    pinMode(pinosEntrada[i], INPUT);
  }

  // Pinos individuais:
  pinMode(VBAT_READ, INPUT);                      // Pino da bateria
  pinMode(PINO_BOTAO_SAIR_SEGURO, INPUT_PULLUP);  // Pino do botão de sair do modo seguro
  pinMode(LED_PIN, OUTPUT);                       // Pino do LED integrado
  pinMode(PINO_VEXT, OUTPUT);                     // Pino Vext
  pinMode(OLED_RESET, OUTPUT);                    // Pino de reset do OLED
  #if (USE_LORA_EXT)
    pinMode(LORA_EXT_M0, OUTPUT);                   // Pino M0 do LoRa externo
    pinMode(LORA_EXT_M1, OUTPUT);                   // Pino M1 do LoRa externo
    pinMode(LORA_EXT_AUX, INPUT_PULLUP);            // Pino AUX do LoRa externo

    digitalWrite(LORA_EXT_M0, LOW);
    digitalWrite(LORA_EXT_M1, LOW);
  #endif

  analogReadResolution(ANALOG_RESOLUTION);
  Serial.println("Pinos configurados.");
}


/**
 * @brief Inicializa o Bluetooth.
 * @return [Boolean] true se o Bluetooth foi iniciado corretamente, false caso contrário.
 */
bool setupBluetooth() {
  // Inicializa Bluetooth
  if (!btStart()) {
    dispmsg("Bluetooth iniciado.", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
    return true;
  } else {
    showError("Falha ao iniciar Bluetooth.", 1);
    sinalizaErro(ERROCRIT_PISCA, "rapido");
    return false;
  }
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
    dispmsg("Display OLED desativado.", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
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

/**
 * @brief Configura a Serial 2 com timeout.
 * @return [Boolean] true se a Serial 2 foi iniciada corretamente, false caso contrário.
 */
bool setupSerial2() {
  unsigned long startMillis = millis();

  Serial2.begin(BAUD_RATE, SERIAL_8N1, SERIAL2_RX_PIN, SERIAL2_TX_PIN);

  while (!Serial2 && millis() - startMillis < SERIAL_TIMEOUT_MS) {
    delay(100);
  }

  if (!Serial2) {
    showError("Serial não iniciada.", 1);
    sinalizaErro(ERROSERIAL_PISCA, "rapido");
    delay(2000);
    return false;
  } else {
    dispmsg("Serial 2 iniciada com sucesso.", 0, 0, 1, SSD1306_WHITE, SSD1306_BLACK, false, true);
    return true;
  }
}

/**
 * @brief Configura o módulo LoRa externo E220.
 * Escreve os parâmetros definidos no módulo e sinaliza sucesso ou falha.
 * @return [Boolean] true se a configuração foi bem-sucedida, false caso contrário.
 */
 bool setupLoRaExt() {
    Serial.println("[E220] Inicializando módulo externo...");

    LoRaExt.begin();
    delay(500);

    Serial.print("[E220] AUX: ");
    Serial.println(digitalRead(LORA_EXT_AUX));

    if (!readParametersE220Bin()) {
        Serial.println("[E220] Falha ao ler configuração binária.");
        return false;
    }

    Serial.println("[E220] Inicialização concluída.");
    return true;
}



bool setupLoRaExtOld() {
    dispmsg("Inicializando LoRa Ext...");

    // Serial2 precisa ter sido iniciada antes desta função.
    LoRaExt.begin();
    delay(500);

    Serial.print("[E220] AUX antes da leitura: ");
    Serial.println(digitalRead(LORA_EXT_AUX));

    ResponseStructContainer rsc = LoRaExt.getConfiguration();

    if (rsc.status.code != E220_SUCCESS || rsc.data == nullptr) {
        Serial.print("[E220] Falha ao ler configuração: ");
        Serial.println(rsc.status.getResponseDescription());

        // Não acessar rsc.data quando for nullptr.
        return false;
    }

    Configuration currentConfig =
        *(Configuration*)rsc.data;

    Serial.println("[E220] Configuração atual lida:");
    printParameters(currentConfig);

    rsc.close();

    // Configuração igual à utilizada no Raspberry.
    currentConfig.ADDH = LORA_ADDRH;
    currentConfig.ADDL = LORA_ADDRL;

    currentConfig.SPED.uartBaudRate = UART_BPS_9600;
    currentConfig.SPED.uartParity = MODE_00_8N1;
    currentConfig.SPED.airDataRate = AIR_DATA_RATE_010_24;

    currentConfig.CHAN = LORA_CHANNEL;

    currentConfig.OPTION.subPacketSetting = SPS_200_00;
    currentConfig.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
    currentConfig.OPTION.transmissionPower = 0b00;  // POWER_30

    currentConfig.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    currentConfig.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
    currentConfig.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
    currentConfig.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;

    ResponseStatus status = LoRaExt.setConfiguration(
        currentConfig,
        WRITE_CFG_PWR_DWN_SAVE
    );

    if (status.code != E220_SUCCESS) {
        Serial.print("[E220] Falha ao gravar configuração: ");
        Serial.println(status.getResponseDescription());
        return false;
    }

    Serial.println("[E220] Configuração gravada com sucesso.");

    delay(500);

    // Confirma a configuração gravada.
    ResponseStructContainer check =
        LoRaExt.getConfiguration();

    if (check.status.code != E220_SUCCESS ||
        check.data == nullptr) {

        Serial.print("[E220] Falha na verificação: ");
        Serial.println(check.status.getResponseDescription());
        return false;
    }

    Configuration savedConfig =
        *(Configuration*)check.data;

    Serial.println("[E220] Configuração confirmada:");
    printParameters(savedConfig);

    check.close();

    Serial.print("[E220] AUX depois da configuração: ");
    Serial.println(digitalRead(LORA_EXT_AUX));

    dispmsg("LoRa Ext iniciado.");
    return true;
}


void testeE220Bruto() {
    Serial.println("[E220] Teste bruto iniciado.");

    digitalWrite(LORA_EXT_M0, HIGH);
    digitalWrite(LORA_EXT_M1, HIGH);
    delay(500);

    while (Serial2.available()) {
        Serial2.read();
    }

    const uint8_t comando[] = {
        0xC1, 0x00, 0x09
    };

    Serial2.write(comando, sizeof(comando));
    Serial2.flush();

    unsigned long inicio = millis();
    uint8_t quantidade = 0;

    Serial.print("[E220] Resposta: ");

    while (millis() - inicio < 1000) {
        while (Serial2.available()) {
            uint8_t valor = Serial2.read();

            if (valor < 0x10) {
                Serial.print("0");
            }

            Serial.print(valor, HEX);
            Serial.print(" ");
            quantidade++;
        }

        delay(5);
    }

    Serial.println();

    Serial.print("[E220] Total de bytes: ");
    Serial.println(quantidade);

    digitalWrite(LORA_EXT_M0, LOW);
    digitalWrite(LORA_EXT_M1, LOW);
    delay(500);
}

#endif
// setup.h