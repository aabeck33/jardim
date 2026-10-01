#include <GardenController.h>
#include <main.h>
#include <setup.h>
#include <utils.h>

void GardenController::begin() {
  systemContext.lastInitStatus = StatusCode::NotInitialized;
  esp_task_wdt_init(WDT_TIMEOUT_MS / 1000, true);
  esp_task_wdt_add(NULL);

  #if (USE_SERIAL)
    systemContext.serialReady = setupSerial();
    if (!systemContext.serialReady) {
      systemContext.lastInitStatus = StatusCode::HardwareFailure;
    }
    Serial.println("Inicializando " + String(NOME_PROJETO) + " - " + String(VERSAO_FIRMWARE));
    Serial.println("Dispositivo: " + String(DISPOSITIVO));
    Serial.println("Tipo: " + String(TIPO_DISPOSITIVO));
  #else
    systemContext.serialReady = false;
    iniciarPinos();
  #endif

  #if (USE_SERIAL_2)
    if (!setupSerial2()) {
      systemContext.lastInitStatus = StatusCode::CommunicationFailure;
    }
  #endif

  #if (USE_DISPLAY && !USE_SERIAL)
    setupDisplay();
  #elif (!USE_DISPLAY && !USE_SERIAL)
    dispmsg("Display OLED desativado.");
  #endif

  if (systemContext.safeMode) {
    systemContext.lastInitStatus = StatusCode::NotInitialized;
    dispmsg("Iniciando em MODO SEGURO - SetUp simplificado.");
    return;
  }

  #if (USE_WIFI)
    setupWiFi();
  #else
    WiFi.mode(WIFI_OFF);
  #endif

  #if (USE_BLUETOOTH)
    if (!setupBluetooth()) {
      systemContext.lastInitStatus = StatusCode::HardwareFailure;
    }
  #else
    btStop();
  #endif

  #if (USE_SPIFFS)
    setupSPIFFS();
  #endif

  #if (USE_LORA)
    setupLoRa();
  #else
    dispmsg("LoRa desativado.");
  #endif

  #if (USE_LORA_EXT)
    loraExtReady_ = setupLoRaExt();
    if (!loraExtReady_) {
      systemContext.lastInitStatus = StatusCode::CommunicationFailure;
      Serial.println("[E220] Inicialização falhou.");
    }
  #else
    dispmsg("LoRaExt desativado.");
  #endif

  #if (USE_DISPLAY)
    systemContext.displayReady = true;
    display.clearDisplay();
    displayOnOff("off");
  #endif

  digitalWrite(LED_PIN, LOW);
  if (systemContext.lastInitStatus == StatusCode::NotInitialized) {
    systemContext.lastInitStatus = StatusCode::Ok;
  }
}

void GardenController::update() {
  esp_task_wdt_reset();

  #if (!TEST_MODE)
    if (systemContext.safeMode) {
      Serial.println("Modo seguro ativo. Aguarde comando para sair.");
      #if (USE_DISPLAY)
        dispmsg("Modo seguro ativo");
      #endif
      sinalizaErro(MODOSEGURO_PISCA, "rapido");
      verificarBotaoModoSeguro();
      delay(100);

      if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        dispmsg("Comando recebido: " + cmd);
        if (cmd == "sair") {
          systemContext.safeMode = false;
          dispmsg("Saindo do modo seguro.");
          delay(100);
          esp_restart();
        } else if (cmd == "status") {
          dispmsg("Modo seguro ativo. Aguardando instruções.");
        }
      }
      return;
    }

    #if (USE_WIFI)
      if (WIFI_MODE == WIFI_STA && WiFi.status() != WL_CONNECTED) {
        if (millis() - ultimaTentativaWiFi > INTERVALO_RECONEXAO_WIFI) {
          dispmsg("Wi-Fi desconectado. Tentando reconectar...");
          ultimaTentativaWiFi = millis();
          connectToWiFi();
        }
      }
    #endif

    String payload = telemetry_.collect();
    payload += '\n';

    #if (USE_ENCRYPTION)
      String encryptedPayload = xorEncrypt(payload, XOR_KEY);
      #if (USE_LORA || USE_LORA_EXT)
        communication_.sendTelemetry(encryptedPayload);
      #endif
    #else
      #if (USE_LORA || USE_LORA_EXT)
        communication_.sendTelemetry(payload);
      #endif
    #endif

    if (systemContext.serialReady || DEBUG_MODE) {
      verificarUsoRAM();
    }

    #if (USE_DEEP_SLEEP)
      dispmsg("DeepSleep por 10 min.");
      #if (USE_SPIFFS && DEBUG_MODE)
        logToSPIFFS("Entrando em modo de sono profundo por 10 minutos...");
      #endif
      esp_sleep_enable_timer_wakeup(TEMPO_ENVIO * 60000000);
      esp_deep_sleep_start();
    #else
      Serial.println("Aguardando 10 minutos antes do próximo envio...");
      #if (USE_DISPLAY)
        dispmsg("Aguardando 10 min.", 0);
        dispmsg("antes do próx. envio.", 1);
        display_.power(false);
      #endif
      aguardar(TEMPO_ENVIO);
    #endif
  #else
    #if (USE_LORA_EXT)
      Serial.print("[E220] AUX antes: ");
      Serial.println(digitalRead(LORA_EXT_AUX));
      String payload =
        "{\"dispositivo\":\"aabeck-01\"," 
        "\"tipo\":\"ESP32V3\"," 
        "\"id\":" + String(systemContext.telemetryCount++) + ","
        "\"temp\":29}\n";
      ResponseStatus status = LoRaExt.sendMessage(payload.c_str(), payload.length());
      Serial.print("[E220] Envio: ");
      Serial.print(status.code);
      Serial.print(" - ");
      Serial.println(status.getResponseDescription());
      Serial.print("[E220] AUX depois: ");
      Serial.println(digitalRead(LORA_EXT_AUX));
      delay(3000);
    #else
      delay(1000);
    #endif
  #endif
}
