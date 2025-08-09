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
#include "main.h"
#include "setup.h"
#include "utils.h"

// Instância do módulo LoRa SX1262
SX1262 lora = new Module(/* NSS  */ 18, 
                         /* DIO1 */ 14, 
                         /* RESET*/ 23, 
                         /* BUSY */ 26);

// Instância do display OLED SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


/**
 * @brief Função de inicialização do sistema.
 * Configura watchdog, inicializa periféricos, sensores, display, LoRa, SPIFFS e Wi-Fi conforme configurações.
 * Também prepara pinos e modo seguro, exibindo mensagens de status.
 */
void setup() {
  // Inicializa o watchdog timer (WDT) para evitar loops infinitos
  esp_task_wdt_init(WDT_TIMEOUT_MS / 1000, true); // o WDT espera segundos
  esp_task_wdt_add(NULL); // adiciona a tarefa atual - setup() - ao WDT

  #if (USE_SERIAL)
    serialOk = setupSerial();

    Serial.println("Inicializando " + String(NOME_PROJETO) + " - " + String(VERSAO_FIRMWARE));
    Serial.println("Dispositivo: " + String(DISPOSITIVO));
    Serial.println("Tipo: " + String(TIPO_DISPOSITIVO));
  #endif

  if (modoSeguro) {
    Serial.println("Iniciando em MODO SEGURO - SetUp simplificado.");
    #if (USE_DISPLAY)
      setupDisplay();
    #endif
    // Não inicializa sensores, LoRa etc.
    // Aguarda comando via serial ou botão para sair do modo seguro
    return;
  }

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
    setupDisplay();
  #else
    Serial.println("Display OLED desativado.");
  #endif

  #if (USE_LORA)
    setupLoRa();
  #else
    Serial.println("LoRa desativado.");
  #endif

  #if (USE_DISPLAY)
    display.clearDisplay();
    displayOnOff("off");
  #endif
}


/**
 * @brief Função principal de execução contínua.
 * Gerencia modo seguro, reconexão Wi-Fi, coleta e envio de dados, exibição de status, uso de memória e modo de sono profundo.
 * Realiza o ciclo principal do dispositivo, incluindo aguardar entre envios.
 */
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
        modoSeguro = false;  // Limpa a flag de modo seguro
        Serial.println("Saindo do modo seguro.");
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

  String payload = coletarDados(); // Coleta os dados e cria o JSON para envio

  #if (USE_ENCRYPTION)
    String encryptedPayload = xorEncrypt(payload, XOR_KEY);   // Encripta os dados para envio
    enviarDados(encryptedPayload);            // Envia os dados encriptados via LoRa
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
      delay(3000);
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
      delay(3000);

      // Desliga o display após o envio
      displayOnOff("off");
    #endif
    aguardar(TEMPO_ENVIO);
  #endif
}
// Fim do código principal