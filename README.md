# Jardim Inteligente

Sistema de irrigacao composto por nos ESP32 e uma Central executada em Raspberry Pi. Os nos coletam dados dos sensores e transmitem telemetria para a Central por LoRa, usando o modulo SX1262 integrado ou o EByte E220 externo.

## Estrutura do projeto

```text
include/
  main.h          Configuracoes, pinos, tipos e declaracoes publicas
  setup.h         Declaracoes de inicializacao dos perifericos
  utils.h         Declaracoes de coleta, telemetria, comandos e suporte
  secrets.h       Credenciais locais do Wi-Fi; nao compartilhar
  AppConfig.h     Flags e parametros da aplicacao
  BoardPins.h     Pinos, radio e parametros especificos da placa
  StatusCode.h    Resultados padronizados de operacoes
  GardenController.h
                  Orquestracao do ciclo de vida da aplicacao
  SystemContext.h Estado operacional compartilhado entre modulos
  interfaces/     Contratos ISensor, IActuator e ITransport
  services/       Fronteiras de display, telemetria e comunicacao

src/
  main.cpp        Ponto de entrada: setup() e loop()
  GardenController.cpp
                  Inicializacao e atualizacao da aplicacao
  setup.cpp       Implementacoes de inicializacao
  utils.cpp       Implementacoes de utilitarios e comunicacao
  DisplayService.cpp
  TelemetryService.cpp
  CommunicationService.cpp

test/
  test_main.cpp   Testes Unity do firmware

central/
  main.py         Aplicacao da Central
  hardware/       GPIO e comunicacao LoRa
  ia/             Treinamento e decisao
  services/       Servicos externos, como clima
  pages/          Interface da Central

lib/AESLib/       Biblioteca local disponivel para futuras implementacoes
                  de criptografia autenticada
data/              Arquivos de dados do PlatformIO
platformio.ini    Ambientes, placa, bibliotecas e flags de compilacao
```

Os arquivos `.h` ficam em `include/` porque sao a interface publica do firmware. As implementacoes ficam em `src/` e sao compiladas uma unica vez. O `GardenController` coordena o fluxo, enquanto os servicos isolam display, telemetria e comunicacao. Os contratos de sensores, atuadores e transportes permitem adicionar implementacoes sem acoplar a aplicacao a um dispositivo concreto.

As configuracoes estao separadas por responsabilidade: `AppConfig.h` contem opcoes e parametros da aplicacao, `BoardPins.h` contem o mapa da placa e `secrets.h` contem apenas credenciais locais.

## Hardware atual

- Placa: Heltec WiFi LoRa 32 V3, ESP32-S3.
- Comunicacao principal atual: EByte E220 externo via Serial2.
- Comunicacao alternativa: SX1262 integrado, desativado por padrao.
- Display: OLED SSD1306 128x64 via I2C.
- Sensores: entradas analogicas de umidade do solo, bateria opcional e temperatura interna.

## Instalacao

### Firmware ESP32

1. Instale o VS Code e a extensao PlatformIO.
2. Abra a pasta raiz deste projeto.
3. Alternativamente, instale o PlatformIO Core:

   ```powershell
   python -m pip install platformio
   ```

4. Preencha `include/secrets.h` apenas se o Wi-Fi for habilitado:

   ```cpp
   #define WIFI_SSID "nome-da-rede"
   #define WIFI_PASSWORD "senha-da-rede"
   ```

   Esse arquivo deve permanecer local e nao deve conter credenciais em commits.

### Central Raspberry Pi

No PowerShell ou terminal Linux:

```powershell
python -m venv central/.venv
central/.venv/Scripts/Activate.ps1
python -m pip install -r central/requirements.txt
```

No Raspberry Pi, use o ativador correspondente ao shell e ao sistema operacional.

## Compilacao e upload

Os comandos podem ser executados pela interface do PlatformIO ou pelo terminal na raiz do projeto.

### Ambiente de comunicacao/teste

Mantem o comportamento atual de teste do E220, com `TEST_MODE=1`:

```powershell
pio run -e heltec_wifi_lora_32_v3
pio run -e heltec_wifi_lora_32_v3 -t upload
pio device monitor -b 9600
```

### Ambiente de producao

Usa o fluxo normal de coleta e envio, com `TEST_MODE=0` e `DEBUG_MODE=0`:

```powershell
pio run -e production
pio run -e production -t upload
pio device monitor -b 9600
```

### Perfil sem recursos opcionais

Valida que o firmware continua compilando com display, SX1262, E220 e Serial2 desativados:

```powershell
pio run -e feature-disabled
```

## Testes

Para compilar o alvo de testes sem gravar nem executar na placa:

```powershell
pio test -e unit-test --without-uploading --without-testing
```

Para executar os testes em uma placa conectada:

```powershell
pio test -e unit-test
```

Os testes atuais exercitam APIs que dependem de GPIO, display, Serial e radio. A execucao completa requer o hardware conectado e configurado.

## Configuracao de recursos

As opcoes podem ser alteradas em `include/main.h` ou sobrescritas por `build_flags` no `platformio.ini`:

- `USE_DISPLAY`: display OLED.
- `USE_LORA`: SX1262 integrado.
- `USE_LORA_EXT`: E220 externo.
- `USE_SERIAL_2`: Serial2 usada pelo E220.
- `USE_WIFI`: Wi-Fi.
- `USE_BLUETOOTH`: Bluetooth.
- `USE_SPIFFS`: armazenamento local.
- `USE_DEEP_SLEEP`: sono profundo.
- `USE_BATTERY`: leitura da bateria.
- `TEST_MODE`: teste de transmissao do E220.
- `DEBUG_MODE`: mensagens adicionais de diagnostico.

O XOR existente e apenas ofuscacao e nao deve ser tratado como protecao criptografica. Para dados sensiveis, e necessario definir um protocolo compativel com a Central usando criptografia autenticada.

## Execucao da Central

Com o ambiente Python ativado:

```powershell
python central/main.py
```

Consulte [central/README.md](central/README.md) para configuracao de GPIO, porta serial, LoRa, dados e IA.

## Comunicacao ESP32 x Central

O firmware transmite telemetria em JSON pelo E220 ou SX1262, conforme o ambiente e as macros habilitadas. A Central recebe os pacotes pela camada `central/hardware/lora/`.

Antes de testar em campo, confirme:

- porta serial e pinos RX/TX;
- pino AUX, M0 e M1 do E220;
- canal, endereco, baud rate e parametros de radio;
- porta serial usada pela Central;
- ambiente `production` ou `heltec_wifi_lora_32_v3` selecionado conscientemente.

## Comandos extensíveis

Os comandos recebidos pelo E220 ou SX1262 sao encaminhados para `CommandProcessor`. Os comandos built-in sao:

- `LED_ON`
- `LED_OFF`
- `SLEEP <segundos>` entre 1 e 86400

Novos comandos podem ser registrados por um modulo usando `registrarComando(nome, handler)`, sem aumentar uma cadeia de `if/else`. Cada handler retorna um `StatusCode`, como `ok`, `invalid_argument`, `communication_failure` ou `unknown_command`.

O ultimo resultado de inicializacao e de comando fica disponível em `SystemContext` por meio de `lastInitStatus` e `lastCommandStatus`.

## Estado atual e proximos passos

O firmware ja possui watchdog, modo seguro, telemetria JSON, dois caminhos de radio e compilacao por perfis. Ainda permanecem como evolucoes futuras a separacao de sensores e atuadores, testes com mocks para hardware, controle de bombas e um protocolo de criptografia autenticada.

Desenvolvido por Alvaro Adriano Beck.
