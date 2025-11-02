/**
 * @file main.cpp
 * @brief Código principal do projeto Jardim Inteligente
 * by Alvaro A. Beck  -  2025-07 - Last update: 2025-00-10
 * This code is licensed under the GNU General Public License v3.0
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * This project is designed to run on an ESP32 board with LoRa capabilities.
 * It collects data from soil moisture sensors, battery voltage, and internal temperature,
 * then sends this data via LoRa in a JSON format. The data is encrypted using AES-128.
 * It also includes a watchdog timer to prevent infinite loops and a display for status updates.
 * This code is designed to be compiled with PlatformIO using the C++11 standard.
 * PlatformIO configuration is set in platformio.ini file.
 * Teste - Simulação de JSON: https://arduinojson.org/v6/assistant/
 * Teste - Simulação de LoRa SX1262: https://radiolib.github.io/radiolib-docs/html/class_s_x1262.html
 * Teste - Simulação de LoRa E220: https://ebyte.readthedocs.io/en/latest/E220/E220.html
 * Teste - Simulação de display OLED SSD1306: https://github.com/adafruit/Adafruit_SSD1306
 * Information: https://randomnerdtutorials.com/
 * https://mischianti.org/ebyte-lora-e220-device-for-arduino-esp32-or-esp8266-manage-wake-on-radio-and-sends-structured-data-5/#google_vignette
 * https://github.com/xreef/EByte_LoRa_E220_Series_Library
 */
#include "main.h"
#include "setup.h"
#include "utils.h"


#if (USE_LORA)
  /**
   * @brief Initializes an SX1262 LoRa module with specified pin configuration.
   *
   * @param NSS   Chip select (NSS) pin number.
   * @param DIO1  DIO1 pin number for interrupt handling.
   * @param RESET Reset pin number for hardware reset.
   * @param BUSY  Busy pin number to monitor module status.
   *
   * @note The SX1262 object is created using the specified pin assignments.
   */
  SX1262 lora = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
#endif

#if (USE_LORA_EXT)
  /**
   * @brief Initializes an E220 LoRa module with specified pin configuration.
   *
   * @param LORA_EXT_M0   M0 pin number for mode selection.
   * @param LORA_EXT_M1   M1 pin number for mode selection.
   * @param LORA_EXT_AUX  AUX pin number for module status monitoring.
   *
   * @note The E220 object is created using the specified pin assignments.
   */
  LoRa_E220 LoRaExt(&Serial2, LORA_EXT_M0, LORA_EXT_M1, LORA_EXT_AUX);
#endif

#if (USE_DISPLAY)
  /**
   * @brief Creates an instance of the Adafruit_SSD1306 display object.
   *
   * This object is used to interface with an SSD1306 OLED display using the I2C protocol.
   *
   * @param SCREEN_WIDTH The width of the display in pixels.
   * @param SCREEN_HEIGHT The height of the display in pixels.
   * @param &Wire Reference to the I2C communication object.
   * @param OLED_RESET The pin used to reset the display (can be set to -1 if not used).
   */
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif


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
    // Inicia a Serial, todos os pinos e o display se necessário.
    serialOk = setupSerial();
    Serial.println("Inicializando " + String(NOME_PROJETO) + " - " + String(VERSAO_FIRMWARE));
    Serial.println("Dispositivo: " + String(DISPOSITIVO));
    Serial.println("Tipo: " + String(TIPO_DISPOSITIVO));
  #else
    serialOk = false;
    // Inicializa pinos
    iniciarPinos();
  #endif

  #if (USE_SERIAL_2)
    setupSerial2();
  #endif

  #if (USE_DISPLAY)
    setupDisplay();
  #else
    dispmsg("Display OLED desativado.");
  #endif

  // Configura modo seguro
  if (modoSeguro) {
    dispmsg("Iniciando em MODO SEGURO - SetUp simplificado.");
    // Não inicializa sensores, LoRa etc.
    // Aguarda comando via serial ou botão para sair do modo seguro
    return;
  }

  #if (USE_WIFI)
    setupWiFi();
  #else
    WiFi.mode(WIFI_OFF);
  #endif

  #if (USE_BLUETOOTH)
    setupBluetooth();
  #else
    btStop();
  #endif

  // Inicializa o SPIFFS para armazenamento de arquivos
  #if (USE_SPIFFS)
    setupSPIFFS();
  #endif

  #if (USE_LORA)
    setupLoRa();
  #else
    dispmsg("LoRa desativado.");
  #endif

  #if (USE_LORA_EXT)
    setupLoRaExt();
  #else
    dispmsg("LoRaExt desativado.");
  #endif

  digitalWrite(LED_PIN, LOW);     // Desliga o LED integrado

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
      dispmsg("Modo seguro ativo");
    #endif
    sinalizaErro(MODOSEGURO_PISCA, "rapido");

    // Verifica se o botão de sair do modo seguro foi pressionado
    verificarBotaoModoSeguro();
    delay(100);
    
    // Espera comando para sair do modo seguro
    if (Serial.available()) {
      String cmd = Serial.readStringUntil('\n');
      cmd.trim(); // Remove espaços em branco
      dispmsg("Comando recebido: " + cmd);
      if (cmd == "sair") {
        modoSeguro = false;  // Limpa a flag de modo seguro
        dispmsg("Saindo do modo seguro.");
        delay(100);
        esp_restart();       // Reinicia no modo normal
      } else if (cmd == "status") {
        dispmsg("Modo seguro ativo. Aguardando instruções.");
      }
    }
    return;  // Evita continuar no loop se estiver em modo seguro
  }
  
  // Verifica se o Wi-Fi está conectado e conecta se necessário
  #if (USE_WIFI)
    if (WIFI_MODE == WIFI_STA && WiFi.status() != WL_CONNECTED) {
      if (millis() - ultimaTentativaWiFi > INTERVALO_RECONEXAO_WIFI) {
        dispmsg("Wi-Fi desconectado. Tentando reconectar...");
        ultimaTentativaWiFi = millis();
        connectToWiFi();
      }
    }
  #endif

  String payload = coletarDados(); // Coleta os dados e cria o JSON para envio

  #if (USE_ENCRYPTION)
    String encryptedPayload = xorEncrypt(payload, XOR_KEY);   // Encripta os dados para envio
    #if USE_LORA
      enviarDados(encryptedPayload);            // Envia os dados encriptados via LoRa
    #endif
  #else
    #if USE_LORA
      enviarDados(payload);
    #endif
  #endif

  // Exibe informações de depuração sobre o uso de memória
  if (serialOk || DEBUG_MODE) {
    verificarUsoRAM();
  }

  #if (USE_DEEP_SLEEP)
    dispmsg("DeepSleep por 10 min.");
    #if (USE_SPIFFS && DEBUG_MODE)
      logToSPIFFS("Entrando em modo de sono profundo por 10 minutos...");
    #endif
    esp_sleep_enable_timer_wakeup(TEMPO_ENVIO * 60000000); // microsegundos
    esp_deep_sleep_start(); // Entra em sono profundo
    // O código não continuará após este ponto, pois o ESP32 reiniciará.
  #else
    Serial.println("Aguardando 10 minutos antes do próximo envio...");
    #if (USE_DISPLAY)
      dispmsg("Aguardando 10 min.", 0);
      dispmsg("antes do próx. envio.", 1);

      // Desliga o display após o envio
      displayOnOff("off");
    #endif
    aguardar(TEMPO_ENVIO);
  #endif
/*
int state = lora.transmit("PING123\n");
  if (state == RADIOLIB_ERR_NONE) {
    dispmsg("Transmissão LoRa OK.");
  } else {
    dispmsg("Erro na transmissão LoRa: " + String(state));
  }
  */
}

// main.cpp