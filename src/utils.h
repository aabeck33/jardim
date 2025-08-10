// utils.h
#ifndef UTILS_H
#define UTILS_H

#include "main.h"


/**
 * @brief Liga ou desliga o display OLED.
 * @param state [String] Estado desejado ("on" ou "off"). Padrão: "on".
 */
void displayOnOff(String state) {
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
void sinalizaErro(uint8_t numPisca, String frequencia) {
  uint32_t tempoDelay;

  if (frequencia == "lento") {
    tempoDelay = 300;
  } else if (frequencia == "rapido") {
    tempoDelay = 100;
  } else {
    tempoDelay = 200; // Padrão
  }
  for (int i = 0; i < numPisca; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(tempoDelay);
    digitalWrite(LED_BUILTIN, LOW);
    delay(tempoDelay);
  }
}


/**
 * @brief Executa rotina de erro crítico, sinaliza e reinicia o ESP32.
 * @param motivo [String] Mensagem do motivo do erro. Padrão: "Erro crítico não especificado".
 */
void erroCritico(String motivo) {
  Serial.println("ERRO CRÍTICO: " + motivo);
  Serial.println("Entrando em modo seguro...");
  modoSeguro = true;    // Seta a flag
  sinalizaErro(ERROCRIT_PISCA, "rapido");
  delay(1000);
  esp_restart();        // Reinicia o ESP32 e entra em modo seguro
}


/**
 * @brief Exibe mensagem de erro no display e/ou Serial.
 * @param message [String] Mensagem de erro.
 * @param errorType [int8_t] Tipo do erro (1: crítico, 2: comunicação, -1: genérico). Padrão: -1.
 */
void showError(String message, int8_t errorType) {
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(6 * 0, 8 * 0);
    if (errorType == 1) {
      display.print("Erro crítico: " + message);
    } else if (errorType == 2) {
      display.print("Erro de comunicação: " + message);
    } else {
      display.print("Erro: " + message);
    }
    display.display();
    delay(3000);
  #else
    if (errorType == 1) {
      Serial.print("Erro crítico: " + message + ". Reinicie o dispositivo.");
    } else if (errorType == 2) {
      Serial.print("Erro de comunicação: " + message);
    } else {
      Serial.print("Erro: " + message);
    }
  #endif
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

  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(0 * 6, 0 * 8);
    display.print("Conectando ao Wi-Fi:");
    display.setCursor(0 * 6, 1 * 8);
    display.print(ssid);
    display.display();
  #endif

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
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Wi-Fi conectado!");
      display.setCursor(0, 8);
      display.print("IP: " + WiFi.localIP().toString());
      display.display();
      delay(3000);
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
  int raw = analogRead(BATTERY_PIN);
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
  
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(6 * 0, 8 * 0);
    display.print("RAM livre: " + String(heapLivre) + " bytes");
    display.display();
    delay(3000);
  #endif

  #if (USE_SPIFFS && DEBUG_MODE)
    logToSPIFFS("Heap livre: " + String(heapLivre) + " bytes");
    logToSPIFFS("RAM interna livre: " + String(heapInterno) + " bytes");
    logToSPIFFS("RAM externa livre: " + String(heap_caps_get_free_size(MALLOC_CAP_SPIRAM)) + " bytes");
  #endif

  if (heapLivre < 10000) { // Se menos de 10KB livre
    Serial.println("⚠️ ALERTA: Memória RAM baixa!");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("⚠️ Memória RAM baixa!");
      display.display();
      delay(3000);
    #endif

    #if (USE_SPIFFS)
      logToSPIFFS("⚠️ Memória RAM baixa!");
    #endif
  } else {
    Serial.println("Memória RAM OK.");

    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("Memória RAM OK.");
      display.display();
      delay(3000);
    #endif
  }
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
    Serial.println("⚠️ ALERTA: Uso de JSON muito alto!");

    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("⚠️ ALERTA: Uso de JSON alto!");
      display.display();
      delay(3000);
    #endif

    #if (USE_SPIFFS)
      logToSPIFFS("⚠️ ALERTA: Uso de JSON muito alto!");
    #endif
  } else {  
    Serial.println("Uso de JSON dentro do limite.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("Uso de JSON OK.");
      display.display();
      delay(3000);
    #endif
  }
}

/**
 * @brief Lê a temperatura interna do ESP32.
 * @return [Float] Temperatura aproximada em graus Celsius.
 */
float getInternalTemperature() {
  // Lê o sensor interno (bruto)
  uint16_t rawTemp = temperatureRead();
  // Pode-se aplicar uma correção/calibração se necessário
  float rawCalibrated = (rawTemp - 32.0) / 1.8 - 5.0; // ajuste estimado

  return rawCalibrated;  // Aproximado, normalmente entre 20°C e 80°C
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
void aguardar(int tempo) {
  unsigned long interval = 1000;    // 1 segundo
  unsigned long elapsed = 0;

  Serial.println("Aguardando " + String(tempo) + " minutos...");
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(6 * 0, 8 * 0);
    display.print("Aguardando " + String(tempo) + " minutos...");
    display.display();
    delay(3000);
  #endif

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
String xorEncrypt(const String &input, char key) {
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
  for (int i = 0; i < numSensoresUmidade; i++) {
    int leitura = analogRead(pinosUmidade[i]);  // 0 (úmido) a 4095 (seco)
    umidade.add(leitura);
  }

  Serial.println("Dados coletados.");
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(6 * 0, 8 * 2);
    display.print("Dados coletados.");
    display.display();
    delay(3000);
  #endif

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
  Serial.println("Enviando dados: " + payload);
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Enviando dados.......");
    display.display();
    delay(3000);
  #endif

  // Enviar via LoRa
  int status = lora.transmit(payload.c_str());
  if (status == RADIOLIB_ERR_NONE) {
    Serial.println("Dados enviados com sucesso.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("Dados enviados.");
      display.display();
      delay(3000);
    #endif
  } else {
    showError(String(status), 2);
  }
}

#endif
// utils.h
// ...existing code...