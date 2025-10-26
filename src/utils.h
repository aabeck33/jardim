#ifndef UTILS_H
#define UTILS_H
/** 
 * @file utils.h
 * @brief Funções utilitárias para o projeto Jardim Inteligente.
 */
#include "main.h"


/**
 * @brief Liga ou desliga o display OLED.
 * @param state [String] Estado desejado ("on" ou "off"). Padrão: "on".
 */
void displayOnOff(const String &state) {
  if (state == "on") {
    displayStatus = true;
    display.ssd1306_command(SSD1306_DISPLAYON);
  } else {
    displayStatus = false;
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}


/**
 * @brief Pisca o LED embutido para sinalizar erro.
 * @param numPisca [Int8] Número de piscadas.
 * @param frequencia [String] Velocidade de piscada: "lento", "rapido". Padrão: "lento".
 */
void sinalizaErro(const uint8_t numPisca, const String &frequencia) {
  uint16_t tempoDelay;

  if (frequencia == "lento") {
    tempoDelay = 300;
  } else if (frequencia == "rapido") {
    tempoDelay = 100;
  } else {
    tempoDelay = 200; // Padrão
  }
  for (int i = 0; i < numPisca; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(tempoDelay);
    digitalWrite(LED_PIN, LOW);
    delay(tempoDelay);
  }
}


/**
 * @brief Exibe mensagem de erro no display e/ou Serial.
 * @param message [String] Mensagem de erro.
 * @param errorType [int8_t] Tipo do erro (
 *      0: Ero crítico
 *      1: Erro grave
 *      2: Erro de comunicação
 *     -1: Erro genérico
 *      Padrão: -1.
 */
void showError(const String &message, const uint8_t errorType) {
  if (errorType == 0) {
    dispmsg("ERRO CRÍTICO: " + message + "\nReiniciando em modo seguro...");
    modoSeguro = true;    // Seta a flag
    sinalizaErro(ERROCRIT_PISCA, "rapido");
    delay(1000);
    esp_restart();        // Reinicia o ESP32 e entra em modo seguro
  } else if (errorType == 1) {
    dispmsg("Erro grave: " + message);
  } else if (errorType == 2) {
    dispmsg("Erro de comunicação: " + message);
  } else {
    dispmsg("Erro: " + message);
  }
}


/**
 * @brief Verifica se o botão de saída do modo seguro foi pressionado.
 */
void verificarBotaoModoSeguro() {
  if (digitalRead(PINO_BOTAO_SAIR_SEGURO) == LOW) {
    Serial.println("Botão pressionado. Saindo do modo seguro.");
    modoSeguro = false;
    esp_restart();
  }
}


/**
 * @brief Tenta conectar ao Wi-Fi e exibe status no display/Serial.
 */
void connectToWiFi() {
  unsigned long startMillis = millis();

  dispmsg("Conectando ao Wi-Fi:");
  dispmsg(ssid, 1);

  WiFi.reconnect();     // força nova tentativa ativa
  while (WiFi.status() != WL_CONNECTED && millis() - startMillis < WIFI_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi conectado com sucesso!");
    Serial.print("IP Local: ");
    Serial.println(WiFi.localIP());
    #if USE_DISPLAY
      dispmsg("Wi-Fi conectado!");
      dispmsg("IP: " + WiFi.localIP().toString(), 1);
    #endif
  } else {
    sinalizaErro(ERRO_WIFI_PISCA, "rapido");
    showError("Falha na conexão Wi-Fi.", 2);
  }
}


/**
 * @brief Lê a tensão da bateria usando um divisor resistivo de 2:1.
 * @return [Float] Tensão da bateria em volts.
 */
float readBatteryVoltage() {
  int raw = analogRead(VBAT_READ);
  float voltage = (raw / 4095.0) * 3.3 * 2.0; // Ajuste conforme divisor
  return voltage;
}


/**
 * @brief Verifica o uso de memória RAM e exibe/loga alertas se necessário.
 */
void verificarUsoRAM() {
  uint32_t heapLivre = ESP.getFreeHeap();
  uint32_t heapInterno = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);

  Serial.printf("[RAM] Heap livre: %u bytes\n", heapLivre);
  Serial.printf("[RAM] RAM interna livre: %u bytes\n", heapInterno);
  Serial.printf("[RAM] RAM externa livre: %u bytes\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  Serial.printf("[RAM] Total RAM livre: %u bytes\n", heapLivre + heapInterno + heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  
  dispmsg("RAM livre: " + String(heapLivre) + " bytes");

  #if (USE_SPIFFS && DEBUG_MODE)
    logToSPIFFS("Heap livre: " + String(heapLivre) + " bytes");
    logToSPIFFS("RAM interna livre: " + String(heapInterno) + " bytes");
    logToSPIFFS("RAM externa livre: " + String(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)) + " bytes");
  #endif

  if (heapLivre < 10000) { // Se menos de 10KB livre
    dispmsg("⚠️ Memória RAM baixa!");

    #if (USE_SPIFFS)
      logToSPIFFS("⚠️ Memória RAM baixa!");
    #endif
  } else {
    dispmsg("Memória RAM OK.");
  }
}

/**
 * @brief Exibe uma mensagem no display e no Serial.
 * @param msg [String] Mensagem a ser exibida.
 * @param linha [uint8_t] Linha no display.
 * @param coluna [uint8_t] Coluna no display.
 * @param tamanho [uint8_t] Tamanho do texto.
 * @param corTexto [uint8_t] Cor do texto.
 * @param corFundo [uint8_t] Cor de fundo.
 */
void dispmsg(const String &msg, const uint8_t linha, const uint8_t coluna, const uint8_t tamanho, 
  const uint8_t corTexto, const uint8_t corFundo, const bool inverter) {
  Serial.println(msg);
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setTextSize(tamanho);
    display.setCursor(6 * coluna, 8 * linha);
    if (inverter) {
      display.setTextColor(corFundo, corTexto);
    } else {
      display.setTextColor(corTexto, corFundo);
    }
    display.print(msg);
    display.display();
    delay(3000);
  #endif
}

/**
 * @brief Verifica o uso de memória JSON e exibe/loga alertas se necessário.
 * @param doc [StaticJsonDocument] Documento JSON a ser verificado.
 */
void verificarUsoJson(const StaticJsonDocument<JSON_DOC_SIZE> &doc) {
  size_t uso = doc.memoryUsage();
  float percentual = (uso * 100.0) / JSON_DOC_SIZE;

  Serial.print("Uso de memória JSON: ");
  Serial.print(uso);
  Serial.print(" bytes (");
  Serial.print(percentual, 1);
  Serial.println("%)");

  // Se ultrapassar limite
  if (percentual > JSON_USAGE_WARNING_PERCENT) {
    dispmsg("⚠️ ALERTA: Uso de JSON alto!");
    #if (USE_SPIFFS)
      logToSPIFFS("⚠️ ALERTA: Uso de JSON muito alto!");
    #endif
  } else {
    dispmsg("Uso de JSON OK.");
  }
}

/**
 * @brief Lê a temperatura interna do ESP32. Normalmente entre 20°C e 80°C
 * @return [Float] Temperatura aproximada em graus Celsius.
 */
float getInternalTemperature(const String &unidade) {
  uint16_t rawTempC = temperatureRead();

  if (unidade == "fahrenheit") {
    float rawTempF = (rawTempC * 1.8) + 32.0;
    return rawTempF;
  } else {
    return rawTempC;
  }
}


/**
 * @brief Processa comandos recebidos via LoRa.
 * @param cmd [String] Comando recebido.
 */
void processarComando(const String &cmd) {
  if (cmd == "LED_ON") {
    digitalWrite(2, HIGH);  // LED no GPIO2
    Serial.println("LED ligado");
  } else if (cmd == "LED_OFF") {
    digitalWrite(2, LOW);
    Serial.println("LED desligado");
  } else if (cmd.startsWith("SLEEP")) {
    int tempo = cmd.substring(6).toInt();
    Serial.print("Dormir por ");
    Serial.print(tempo);
    Serial.println(" segundos");
    delay(100);
    esp_sleep_enable_timer_wakeup((uint64_t)tempo * 1000000ULL);
    esp_deep_sleep_start();
  } else {
    Serial.println("Comando desconhecido");
  }
}


/**
 * @brief Recebe comandos via LoRa e processa.
 */
void receberComandoLoRa() {
  String recebido;

  int state = lora.receive(recebido);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.print("Comando recebido: ");
    Serial.println(recebido);

    processarComando(recebido);

  } else if (state != RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.print("Erro ao receber: ");
    Serial.println(state);
  }
}


/**
 * @brief Aguarda um tempo específico em minutos, alimentando o watchdog.
 * @param tempo [Int] Tempo em minutos para aguardar.
 */
void aguardar(const uint8_t tempo) {
  unsigned long interval = 1000;    // 1 segundo
  unsigned long elapsed = 0;

  dispmsg("Aguardando " + String(tempo) + " minutos...");

  while (elapsed < tempo * 60000) {
    esp_task_wdt_reset(); // Alimenta o watchdog
    delay(interval);
    elapsed += interval;
    #if (RECEIVE_COMMANDS)
      receberComandoLoRa();
    #endif
  }
}


/**
 * @brief Encripta uma string usando XOR simples.
 * @param input [String] String a ser encriptada.
 * @param key [char] Chave de encriptação (caractere).
 * @return [String] String encriptada.
 */
String xorEncrypt(const String &input, const char key) {
  String output = input;
  for (size_t i = 0; i < input.length(); i++) {
    output[i] = input[i] ^ key;
  }
  return output;
}


/**
 * @brief Descriptografa uma string encriptada com XOR simples.
 * @param input [String] String encriptada.
 * @param key [char] Chave de encriptação (caractere).
 * @return [String] String original descriptografada.
 */
String xorDecrypt(const String &input, const char key) {
  String output = input;
  for (size_t i = 0; i < input.length(); i++) {
    output[i] = input[i] ^ key;
  }
  return output;
}


/**
 * @brief Coleta dados do dispositivo (sensores) e cria um JSON.
 * @return [String] JSON com os dados serializados.
 */
String coletarDados() {
  // Criação do JSON
  StaticJsonDocument<JSON_DOC_SIZE> dados;

  // Identificação e timestamp
  dados["dispositivo"] = DISPOSITIVO;
  dados["tipo"] = TIPO_DISPOSITIVO;
  dados["versao"] = VERSAO_FIRMWARE; 
  unsigned long timestamp = millis();
  dados["timestamp"] = timestamp;  // Tempo desde o boot (ms)

  // Ler temperatura interna do ESP32
  float temperatura = getInternalTemperature();
  dados["temperatura"] = temperatura;

  // Ler a tensão da bateria
  float bateria = readBatteryVoltage();
  dados["bateria"] = bateria;

  // Criar o array para os valores de umidade
  JsonArray umidade = dados.createNestedArray("umidade");
  // Ler sensores de umidade do solo
  for (int i = 0; i < numEntradas; i++) {
    int leitura = analogRead(pinosEntrada[i]);  // 0 (úmido) a 4095 (seco)
    umidade.add(leitura);
  }

  dispmsg("Dados coletados.");

  // Verifica uso de memória do JSON
  verificarUsoJson(dados);

  // Serializa para string
  String payload;
  serializeJson(dados, payload);
  return payload;
}


/**
 * @brief Loga uma mensagem no SPIFFS.
 * @param message [String] Mensagem a ser logada.
 */
void logToSPIFFS(const String &message) {
  File file = SPIFFS.open("/log.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Erro ao abrir arquivo para log");
    return;
  }
  file.println(message);
  file.close();
}


/**
 * @brief Exibe o conteúdo do log armazenado no SPIFFS.
 */
void printLog() {
  File file = SPIFFS.open("/log.txt");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}


/**
 * @brief Envia os dados coletados via LoRa.
 * @param payload [String] JSON com os dados a serem enviados.
 */
void enviarDados(const String &payload) {
  dispmsg("Enviando dados.......");
  Serial.println(payload);

  // Enviar via LoRa
  int status = lora.transmit(payload.c_str());
  if (status == RADIOLIB_ERR_NONE) {
    dispmsg("Dados enviados.");
  } else {
    showError(String(status), 2);
  }
}

/**
 * @brief Faz o reset do display OLED.
 */
void resetOLED() {
  digitalWrite(OLED_RESET, LOW);
  delay(150);
  digitalWrite(OLED_RESET, HIGH);
  delay(150);
  Serial.println("Display resetado.");
}

/**
 * @brief Liga ou Desliga o circuito Vext.
 */
void VextOnOff(const String &state) {
  if (state == "On") {
    digitalWrite(PINO_VEXT, LOW);
    delay(150);
    Serial.println("Circuito Vext ligado.");
  } else {
    digitalWrite(PINO_VEXT, HIGH);
    delay(150);
    Serial.println("Circuito Vext desligado.");
  }
}


/**
 * @brief Aguarda o pino AUX do módulo LoRa ficar HIGH.
 */
void wait_aux_high() {
  while (digitalRead(LORA_EXT_AUX) == LOW) {
    delay(10);
  }
}


/**
 * @brief Configura o modo do módulo LoRa externo E220.
 * @param mode [String] Modo desejado ("normal", "config", "wor-tx", "wor-rx").
 */
void set_mode(const String &mode) {
  if (mode == "normal") {
    #if (DEBUG_MODE)
      dispmsg("[E220] Entrando em modo NORMAL", 1);
    #endif
    digitalWrite(LORA_EXT_M0, LOW);
    digitalWrite(LORA_EXT_M1, LOW);
  } else if (mode == "config") {
    #if (DEBUG_MODE)
      dispmsg("[E220] Entrando em modo CONFIGURAÇÃO/Sleep", 1);
    #endif
    digitalWrite(LORA_EXT_M0, HIGH);
    digitalWrite(LORA_EXT_M1, HIGH);
  } else if (mode == "wor-tx") {
    #if (DEBUG_MODE)
      dispmsg("[E220] Entrando em modo Wake-on-Radio - Transmissão (WOR-TX)", 1);
    #endif
    digitalWrite(LORA_EXT_M0, LOW);
    digitalWrite(LORA_EXT_M1, HIGH);
  } else if (mode == "wor-rx") {
    #if (DEBUG_MODE)
      dispmsg("[E220] Entrando em modo Wake-on-Radio - Recepção (WOR-RX)", 1);
    #endif
    digitalWrite(LORA_EXT_M0, HIGH);
    digitalWrite(LORA_EXT_M1, LOW);
  }
  wait_aux_high();
}


/**
 * @brief Lê os parâmetros do módulo LoRa externo E220.
 * @param ser [HardwareSerial&] Instância da Serial usada para comunicação com o módulo.
 * @return [uint8_t*] Ponteiro para os parâmetros lidos (array de 12 bytes).
 */
uint8_t* read_parameters(HardwareSerial &ser) {
  uint8_t cmd[] = {0xC1, 0x00, 0x09};   // Comando de leitura (9 bytes de dados a partir do endereço 0x00)
  static uint8_t resp[12];              // Array para armazenar a resposta
  memset(resp, 0, sizeof(resp));
  int i = 0;

  set_mode("config");
  #if (DEBUG_MODE)
    dispmsg("[E220] Limpando buffers...", 1);
  #endif
  while (ser.available()) {
    ser.read();
  }
  wait_aux_high();

  #if (DEBUG_MODE)
    dispmsg("[E220] Enviando comando de leitura...", 1);
  #endif
  ser.write(cmd, 3);
  delay(100);

  while (ser.available() && i < 12) {
    resp[i++] = Serial2.read();
  }
  #if (DEBUG_MODE)
    size_t tamanho = i;
    Serial.print("Resposta bruta: ");
    for (int i = 0; i < tamanho; i++) {       // 'tamanho' é o número de bytes válidos em resp
        if (resp[i] < 16) Serial.print("0");  // para sempre ter dois dígitos
        Serial.print(resp[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
  #endif

  set_mode("normal");

  // A resposta de sucesso para leitura é 12 bytes: 0xC1 0x00 0x09 + 8 bytes de dados + 1 byte extra 0x10
  
  if (i == 12 && resp[0] == 0xC1 && resp[1] == 0x00 && resp[2] == 0x09) {
    // Extrai parâmetros
    uint8_t* params = &resp[3];
    uint8_t addh    = params[0];
    uint8_t addl    = params[1];
    uint8_t speed   = params[2];
    uint8_t option  = params[3];
    uint8_t chan    = params[4];
    uint8_t crypt_h = params[6];
    uint8_t crypt_l = params[7];

    // Endereço do dispositivo 2 bytes à partir da posição 0
    uint16_t address = (addh << 8) | addl;
    // Baud rate 3 bits (5-7) a partir da posição 2
    // Air data rate 3 bits (0-2) a partir da posição 2
    // Paridade 2 bits (3-4) a partir da posição 2
    // Canal 7 bits a partir da posição 3
    chan = chan & 0x7F;
    // Frequência aproximada em MHz = 850.125 + canal
    float freq = 850.125 + chan;
    // Potência TX 2 bits (6-7) a partir da posição 4

    // Tabelas de conversão (exemplo, adapte conforme seu código)
    int baud_rates[8]    = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
    const char* parities[4] = {"8N1", "8O1", "8E1", "8N1"};
    float air_rates[8]   = {2.4, 2.4, 2.4, 4.8, 9.6, 19.2, 38.4, 62.5};
    int tx_powers[4]    = {30, 27, 24, 21};

    int baud_rate   = baud_rates[(speed >> 5) & 0b111];
    const char* parity = parities[(speed >> 3) & 0b11];
    float air_data_rate = air_rates[speed & 0b111];
    int tx_power   = tx_powers[option & 0b11];

    Serial.println("\n--- [E220] Configurações Atuais do Módulo ---");
    Serial.print(" Endereço: "); Serial.println(address, HEX);
    Serial.print(" Baud Rate (UART): "); Serial.print(baud_rate); Serial.println(" bps");
    Serial.print(" Paridade: "); Serial.println(parity);
    Serial.print(" Air Data Rate: "); Serial.print(air_data_rate); Serial.println(" kbps");
    Serial.print(" Canal: "); Serial.println(chan);
    Serial.print(" Frequência: "); Serial.print(freq); Serial.println(" MHz");
    Serial.print(" Potência TX: "); Serial.print(tx_power); Serial.println(" dBm");
    Serial.println("-------------------------------------------\n");

    return resp;
    } else {
      Serial.println("[E220] Falha ao ler parâmetros");
      return NULL;
    }
}


/**
 * @brief Escreve os parâmetros no módulo LoRa externo E220.
 * @param params [uint8_t[8]] Array com os 8 bytes de parâmetros a serem escritos.
 * @return [bool] true se a escrita foi bem-sucedida, false caso contrário.
 */
bool write_parameters(uint8_t params[8]) {
  set_mode("config");
  wait_aux_high();

  uint8_t cmd[11];
  cmd[0] = 0xC0;
  cmd[1] = 0x00;
  cmd[2] = 0x08;
  for (int i = 0; i < 8; i++) cmd[3 + i] = params[i];

  Serial2.write(cmd, 11);
  delay(200);

  uint8_t resp[11];
  int i = 0;
  while (Serial2.available() && i < 11) {
    resp[i++] = Serial2.read();
  }

  set_mode("normal");

  if (i == 11 && resp[0] == 0xC1) {
    Serial.println("Parâmetros escritos com sucesso!");
    return true;
  } else {
    Serial.println("Falha ao escrever parâmetros!");
    return false;
  }
}

#endif
// utils.h