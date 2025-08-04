// Projeto Jardim Inteligente
// by Alvaro A. Beck  -  2025-07 - Last update: 2025-07-15
// This code is licensed under the GNU General Public License v3.0
// https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////////////////////////
// Projeto Jardim Inteligente - Código fonte principal
// This project is designed to run on an ESP32 board with LoRa capabilities.
// It collects data from soil moisture sensors, battery voltage, and internal temperature,
// then sends this data via LoRa in a JSON format. The data is encrypted using AES-128.
// Libraries used:
// - RadioLib for LoRa communication
// - ArduinoJson for JSON serialization
// - AESLib for AES encryption
// - Wire for I2C communication (if needed for other sensors in the future)
// - Adafruit BusIO for I2C communication with Adafruit devices (if needed)
// This code is designed to be compiled with PlatformIO using the C++11 standard.
// PlatformIO configuration is set in platformio.ini file.
// Teste - Simulação de JSON: https://arduinojson.org/v6/assistant/
//
//////////////////////////////////////////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <RadioLib.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_task_wdt.h>
#include <SPIFFS.h>
#include <WiFi.h>


// Nome/Tipo do dispositivo (identificação)
constexpr const char* NOME_PROJETO = "Jardim Inteligente";
constexpr const char* DISPOSITIVO = "aabeck-01";
constexpr const char* TIPO_DISPOSITIVO = "ESP32-Jardim";
constexpr const char* VERSAO_FIRMWARE = "1.0.0";
constexpr const char* ssid = "aabeck-ESP32";
constexpr const char* password = "EbSePc3k2&";
constexpr int8_t OLED_RESET = -1;                     // Reset por software
constexpr size_t JSON_DOC_SIZE = 512;                 // Tamanho alocado
constexpr size_t JSON_USAGE_WARNING_PERCENT = 85;     // Percentual de uso que aciona o alerta
constexpr size_t TEMPO_ENVIO = 10;                    // Tempo de envio em minutos
constexpr uint8_t XOR_KEY = 0x5A;                     // Chave de encriptação XOR simples
constexpr uint8_t SCREEN_ADDRESS = 0x3C;              // Endereço I2C do OLED
constexpr uint8_t SCREEN_WIDTH = 128;                 // Largura do OLED
constexpr uint8_t SCREEN_HEIGHT = 64;                 // Altura do OLED
constexpr uint8_t BATTERY_PIN = 34;                   // Pino analógico para monitoramento da bateria
constexpr uint32_t BAUD_RATE = 115200;                // Taxa de transmissão da Serial
constexpr uint16_t SERIAL_TIMEOUT_MS = 5000;          // Timeout da Serial em milissegundos
//constexpr uint8_t LED_BUILTIN = 2;                    // Pino do LED embutido (GPIO 2)
// === Sensores de umidade do solo ===
constexpr int pinosUmidade[] = {25, 32, 33, 35, 36, 39};  // Pinos ADC disponíveis no ESP32
constexpr size_t numSensoresUmidade = sizeof(pinosUmidade) / sizeof(pinosUmidade[0]);
// Obs: Evite usar GPIOs 34 a 39 para saída digital, eles são apenas de entrada analógica.
constexpr uint8_t PINO_BOTAO_SAIR_SEGURO = 27;
// Configuração do LoRa
constexpr float freqLoRa = 915.0;   // Frequência em MHz - Banda ISM para América do Sul
constexpr int txPower = 14;         // Potência de transmissão (em dBm) — limite ANATEL é 20 dBm
// Configuração do watchdog
constexpr uint32_t WDT_TIMEOUT_MS = 60000;            // Timeout do watchdog em milissegundos (1 minuto)
// Configuração do SPIFFS
constexpr const char* SPIFFS_MOUNT_POINT = "/spiffs"; // Ponto de montagem do SPIFFS
// Configuração do Wi-Fi
constexpr wifi_mode_t WIFI_MODE = WIFI_STA;           // Modo Wi-Fi: WIFI_STA (cliente), WIFI_AP (ponto de acesso) ou WIFI_AP_STA (ambos)
constexpr uint32_t INTERVALO_RECONEXAO_WIFI = 180000; // Intervalo de reconexão Wi-Fi em milissegundos
constexpr uint16_t WIFI_TIMEOUT = 10000;              // Timeout do Wi-Fi em milissegundos

// Configurações do programa
#define DEBUG_MODE false        // Modo de depuração
#define USE_DISPLAY true        // Usar display OLED
#define USE_LORA true           // Usar LoRa para comunicação
#define USE_BATTERY false       // Usar monitoramento da bateria
#define USE_ENCRYPTION true     // Usar encriptação
#define USE_DEEP_SLEEP false    // Usar sono profundo para economia de energia
#define USE_SPIFFS false        // Usar SPIFFS para armazenamento de arquivos
#define USE_WIFI false          // Usar Wi-Fi para comunicação
#define USE_SERIAL true         // Usar Serial para depuração
#define RECEIVE_COMMANDS false  // Receber comandos via LoRa - Não usar com USE_DEEP_SLEEP
// Lista de piscadas de LED:
#define ERROCRIT_PISCA 10       // 10 piscadas rápidas
#define ERRODISPLAY_PISCA 3     // 3 piscadas rápidas
#define ERROLORA_PISCA 4        // 4 piscadas rápidas
#define ERROSPIFFS_PISCA 5      // 5 piscadas rápidas
#define ERROSERIAL_PISCA 6      // 6 piscadas rápidas
#define ERRO_WIFI_PISCA 7       // 7 piscadas rápidas
#define MODOSEGURO_PISCA 12     // 12 piscadas rápidas

// LoRa SX1262 (ajuste se necessário)
SX1262 lora = new Module(/* NSS  */ 18, 
                         /* DIO1 */ 14, 
                         /* RESET*/ 23, 
                         /* BUSY */ 26);

// Instância do display OLED SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Atribui valor persistente mesmo após deep sleep (mantido na RAM RTC)
RTC_DATA_ATTR bool modoSeguro = false;  // Modo seguro para evitar loops infinitos
bool displayStatus = false;             // Status do display OLED
bool serialOk = false;                  // Indica se a Serial foi iniciada corretamente

#if (USE_WIFI)
  unsigned long ultimaTentativaWiFi = 0; // Armazena o tempo da última tentativa de conexão Wi-Fi
#endif

void displayOnOff(String state = "on") {
  if (state == "on") {
    displayStatus = true;
    display.ssd1306_command(SSD1306_DISPLAYON);
  } else {
    displayStatus = false;
    display.ssd1306_command(SSD1306_DISPLAYOFF);
  }
}

void sinalizaErro(uint8_t numPisca, String frequencia = "lento") {
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

void erroCritico(String motivo = "Erro crítico não especificado") {
  Serial.println("ERRO CRÍTICO: " + motivo);
  Serial.println("Entrando em modo seguro...");
  modoSeguro = true;    // Seta a flag
  sinalizaErro(ERROCRIT_PISCA, "rapido");
  delay(1000);          // Aguarda 1 segundo para estabilizar
  esp_restart();        // Reinicia o ESP32
}

void showError(String message, int8_t errorType = -1) {
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    // Cada caractere ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
    display.setCursor(6 * 0, 8 * 0);
    if (errorType == 1) {
      display.print("Erro crítico: " + message);
    } else if (errorType == 2) {
      display.print("Erro de comunicação: " + message);
    } else {
      display.print("Erro: " + message);
    }
    display.display();
    delay(3000); // Aguarda 3 segundos para leitura do erro
  #else
    if (errorType == 1) {
      Serial.print("Erro crítico: " + message + ". Reinicie o dispositivo.");
    } else if (errorType == 2) {
      Serial.print("Erro de comunicação: " + message);
    } else {
      Serial.print("Erro: " + message);
  #endif
}

void verificarBotaoModoSeguro() {
  if (digitalRead(PINO_BOTAO_SAIR_SEGURO) == LOW) {
    Serial.println("Botão pressionado. Saindo do modo seguro.");
    modoSeguro = false;
    esp_restart();
  }
}

bool setupSerial() {
  unsigned long startMillis = millis();

  Serial.begin(BAUD_RATE);

  while (!Serial && millis() - startMillis < SERIAL_TIMEOUT_MS) {
    delay(100); // Aguarda a conexão serial
  }

  if (!Serial) {
    showError("Serial não iniciada.", 1);
    sinalizaErro(ERROSERIAL_PISCA, "rapido");
    delay(3000); // Aguarda 3 segundos.
    /*
    while (true) {
      sinalizaErro(ERROSERIAL_PISCA, "rapido");
      delay(3000); // Aguarda 3 segundos.
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
      delay(3000); // Aguarda 3 segundos
    #endif
    return true;
  }
}

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

  WiFi.reconnect();               // força nova tentativa ativa
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
    WiFi.setSleep(false); // Ativa/Desativa o modo de sono do Wi-Fi
    WiFi.setAutoReconnect(true); // Habilita reconexão automática
    WiFi.setAutoConnect(true); // Habilita conexão automática
    WiFi.setHostname(DISPOSITIVO); // Define o hostname do dispositivo
    WiFi.begin(ssid, password); // Conecta como cliente
    //connectToWiFi(); // Faz a conexão se for estação
  }
}

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
      delay(3000); // Aguarda 3 segundos
    #endif
  }
}

void setupLoRa() {
  // Inicializa o LoRa
  Serial.println("Inicializando LoRa...");
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    // Cada caractere ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
    display.setCursor(6 * 0, 8 * 0);
    display.print("Inicializando LoRa...");
    display.display();
    delay(3000); // Aguarda 3 segundos
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
      // Cada caractere ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
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
  delay(3000); // Aguarda 3 segundos para estabilizar
}

void setupDisplay() {
  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    showError("Falha ao inicializar o display OLED.", 1);
    while (true) {
      showError("Display não iniciado.", 1);
      sinalizaErro(ERRODISPLAY_PISCA, "rapido");
      delay(3000); // Aguarda 3 segundos.
    }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // Cada caractere ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
  display.setCursor(6 * 0, 8 * 0);
  Serial.println("Display OLED iniciado.");
  display.print("OLED iniciado.");
  display.display();
  delay(3000); // Aguarda 3 segundos
}

// Função para ler a tensão da bateria (supondo divisor resistivo de 2:1)
float readBatteryVoltage() {
  int raw = analogRead(BATTERY_PIN);
  float voltage = (raw / 4095.0) * 3.3 * 2.0; // Ajuste conforme divisor
  return voltage;
}

// Função para verificar uso de memória RAM
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
    delay(3000); // Aguarda 3 segundos para leitura
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
      delay(3000); // Aguarda 3 segundos para leitura do alerta
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
      delay(3000); // Aguarda 3 segundos para leitura do status
    #endif
  }
}

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
      delay(3000); // Aguarda 3 segundos para leitura do alerta
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
      delay(3000); // Aguarda 3 segundos para leitura do status
    #endif
  }
}

// Função para ler a temperatura interna do ESP32
float getInternalTemperature() {
  // Lê o sensor interno (bruto)
  uint16_t rawTemp = temperatureRead();
  // Pode-se aplicar uma correção/calibração se necessário
  float rawCalibrated = (rawTemp - 32.0) / 1.8 - 5.0; // ajuste estimado

  return rawCalibrated;  // Aproximado, normalmente entre 20°C e 80°C
}

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
    delay(3000); // Aguarda 3 segundos para leitura
  #endif

  while (elapsed < tempo * 60000) {
    esp_task_wdt_reset(); // Alimenta o watchdog
    delay(interval);
    elapsed += interval;
    #if (RECEIVE_COMMANDS)
      receberComandoLoRa(); // Recebe comandos via LoRa
    #endif
  }
}

String xorEncrypt(const String &input, char key) {
  String output = input;
  for (size_t i = 0; i < input.length(); i++) {
    output[i] = input[i] ^ key;
  }
  return output;
}

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
    // Cada caractere ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
    display.setCursor(6 * 0, 8 * 2);
    display.print("Dados coletados.");
    display.display();
    delay(3000); // Aguarda 3 segundos para leitura
  #endif

  // Verifica uso de memória do JSON
  verificarUsoJson(dados);

  // Serializa para string
  String payload;
  serializeJson(dados, payload);
  return payload;
}

void logToSPIFFS(const String &message) {
  File file = SPIFFS.open("/log.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Erro ao abrir arquivo para log");
    return;
  }
  file.println(message);
  file.close();
}

void printLog() {
  File file = SPIFFS.open("/log.txt");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void enviarDados(const String &payload) {
  Serial.println("Enviando dados: " + payload);
  #if (USE_DISPLAY)
    displayOnOff();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Enviando dados.......");
    display.display();
    delay(3000); // Aguarda 3 segundos para leitura
  #endif

  // Enviar via LoRa
  int status = lora.transmit(payload.c_str());
  if (status == RADIOLIB_ERR_NONE) {
    Serial.println("Dados enviados com sucesso.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      // Cada caractere ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas
      display.setCursor(6 * 0, 8 * 0);
      display.print("Dados enviados.");
      display.display();
      delay(3000); // Aguarda 3 segundos para leitura
    #endif
  } else {
    showError(String(status), 2);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Inicializa o watchdog timer (WDT) para evitar loops infinitos
  esp_task_wdt_init(WDT_TIMEOUT_MS / 1000, true); // o WDT espera segundos
  esp_task_wdt_add(NULL); // adiciona a tarefa atual - loop() - ao WDT

  // Inicializa a Serial para depuração
  #if (USE_SERIAL)
    serialOk = setupSerial();

    Serial.println("Inicializando " + String(NOME_PROJETO) + " - " + String(VERSAO_FIRMWARE));
    Serial.println("Dispositivo: " + String(DISPOSITIVO));
    Serial.println("Tipo: " + String(TIPO_DISPOSITIVO));
  #endif

  if (modoSeguro) {
    Serial.println("Iniciando em MODO SEGURO - SetUp simplificado.");
    #if (USE_DISPLAY)
      setupDisplay(); // Configura o display OLED
    #endif
    // Não inicializa sensores, LoRa etc.
    // Aguarda comando via serial ou botão para sair do modo seguro
    return;
  }

  // Configura o Wi-Fi se necessário
  #if (USE_WIFI)
    setupWiFi();
  #endif

  // Inicializa o SPIFFS para armazenamento de arquivos
  #if (USE_SPIFFS)
    setupSPIFFS();
  #endif

  // Um ADC (Conversor Analógico-Digital) de 12 bits gera valores de 0 a 4095 (2¹² - 1).
  // Portanto, se sua tensão de referência for 3.3V, o valor 4095 representa 3.3V, e 0 representa 0V.
  // Cada unidade no valor representa cerca de 0.0008V (3.3V ÷ 4096).
  analogReadResolution(12);

  // Inicializa pinos
  for (int i = 0; i < numSensoresUmidade; i++) {
    pinMode(pinosUmidade[i], INPUT);
  }
  pinMode(BATTERY_PIN, INPUT); // Pino da bateria
  pinMode(PINO_BOTAO_SAIR_SEGURO, INPUT_PULLUP); // Pino do botão de sair do modo seguro
  pinMode(LED_BUILTIN, OUTPUT); // LED integrado do ESP32
  digitalWrite(LED_BUILTIN, LOW); // Desliga o LED integrado
  Serial.println("Pinos configurados.");

  #if (USE_DISPLAY)
    setupDisplay(); // Configura o display OLED
  #else
    Serial.println("Display OLED desativado.");
  #endif

  #if (USE_LORA)
    setupLoRa(); // Configura o LoRa
  #else
    Serial.println("LoRa desativado.");
  #endif

  #if (USE_DISPLAY)
    display.clearDisplay();
    displayOnOff("off");
  #endif
}


void loop() {
  // Reseta o watchdog timer a cada iteração do loop
  esp_task_wdt_reset();

  if (modoSeguro) {
    Serial.println("Modo seguro ativo. Aguarde comando para sair.");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Modo seguro ativo");
      display.display();
    #endif
    sinalizaErro(MODOSEGURO_PISCA, "rapido");

    // Verifica se o botão de sair do modo seguro foi pressionado
    verificarBotaoModoSeguro();
    delay(100);
    // Espera comando para sair do modo seguro
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim(); // Remove espaços em branco
      Serial.println("Comando recebido: " + cmd);
      if (cmd == "sair") {
        modoSeguro = false;  // Limpa a flag
        delay(100);
        esp_restart();       // Reinicia no modo normal
      } else if (cmd == "status") {
        Serial.println("Modo seguro ativo. Aguardando instruções.");
      }
    }
    return;  // Evita continuar no loop se estiver em modo seguro
  }
  
  // Verifica se o Wi-Fi está conectado e conecta se necessário
  #if (USE_WIFI)
    if (WIFI_MODE == WIFI_STA && WiFi.status() != WL_CONNECTED) {
      if (millis() - ultimaTentativaWiFi > INTERVALO_RECONEXAO_WIFI) {
        Serial.println("Wi-Fi desconectado. Tentando reconectar...");
        ultimaTentativaWiFi = millis();
        connectToWiFi();
      }
    }
  #endif

  String payload = coletarDados(); // Coleta os dados e cria o JSON

  #if (USE_ENCRYPTION)
    String encryptedPayload = xorEncrypt(payload, XOR_KEY); // Encripta os dados com XOR simples
    enviarDados(encryptedPayload);
  #else
    enviarDados(payload);
  #endif
  
  // Exibe informações de depuração sobre o uso de memória
  if (serialOk || DEBUG_MODE) {
    verificarUsoRAM();
  }

  #if (USE_DEEP_SLEEP)
    Serial.println("Entrando em modo de sono profundo por 10 minutos...");
    #if (USE_SPIFFS && DEBUG_MODE)
      logToSPIFFS("Entrando em modo de sono profundo por 10 minutos...");
    #endif
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("DeepSleep por 10 min.");
      display.display();
      delay(3000); // Aguarda 3 segundos para leitura
    #endif
    esp_sleep_enable_timer_wakeup(TEMPO_ENVIO * 60000000); // microsegundos
    esp_deep_sleep_start(); // Entra em sono profundo
    // O código não continuará após este ponto, pois o ESP32 reiniciará.
  #else
    Serial.println("Aguardando 10 minutos antes do próximo envio...");
    #if (USE_DISPLAY)
      displayOnOff();
      display.clearDisplay();
      display.setCursor(6 * 0, 8 * 0);
      display.print("Aguardando 10 min.");
      display.setCursor(6 * 0, 8 * 1);
      display.print("antes do próx. envio.");
      display.display();
      delay(3000); // Aguarda 3 segundos para leitura

      // Desliga o display após o envio
      displayOnOff("off");
    #endif
    aguardar(TEMPO_ENVIO);
  #endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// Fim do código principal
//////////////////////////////////////////////////////////////////////////////////////////////////