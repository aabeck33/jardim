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
  sensors/SensorConfig.h
                  Lista explicita de sensores, tipos, pinos e calibracao
  actuators/ActuatorConfig.h
                  Lista explicita de bombas e valvulas
  GardenController.h
                  Orquestracao do ciclo de vida da aplicacao
  SystemContext.h Estado operacional compartilhado entre modulos
  interfaces/     Contratos ISensor, IActuator e ITransport
  services/       Fronteiras de display, telemetria e comunicacao
  sensors/        Leituras e calibracao de sensores
  actuators/      Bombas e gerenciamento de atuadores

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

O `SensorRegistry` registra seis sensores de solo, a temperatura interna e a bateria. As classes `AirHumiditySensor` e `ReservoirLevelSensor` ja estao preparadas para drivers e pinos especificos, mas nao sao ativadas automaticamente sem hardware configurado.

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

### Ambiente de depuracao

Usa o fluxo normal com `TEST_MODE=0` e `DEBUG_MODE=1`:

```powershell
pio run -e debug
pio run -e debug -t upload
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

As opcoes da aplicacao podem ser alteradas em `include/AppConfig.h` ou sobrescritas por `build_flags` no `platformio.ini`. Pinos e parametros especificos da placa ficam em `include/BoardPins.h`.

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

## Configuracao de sensores

Todos os sensores incluidos no firmware devem aparecer explicitamente em `include/sensors/SensorConfig.h`. A entrada de configuracao define nome, tipo, origem, habilitacao, pino e pontos de calibracao:

```cpp
{
  "id",
  SensorType::SoilMoisture,
  SensorSource::AnalogPin,
  true,
  GPIO,
  PONTO_MOLHADO,
  PONTO_SECO
}
```

### Campos da configuracao

- `id`: nome unico enviado no JSON, como `solo_1`.
- `SensorType`: tipo funcional, como `SoilMoisture`, `Battery` ou `ReservoirLevel`.
- `SensorSource`: origem da leitura: `AnalogPin`, `InternalEsp32` ou `ExternalDriver`.
- `enabled`: controla se o sensor sera registrado e enviado.
- `pin`: GPIO fisico. Para sensores internos use `SENSOR_NO_PIN`.
- `firstCalibrationPoint` e `secondCalibrationPoint`: referencias usadas pela conversao do valor bruto.

### Sensores que ja fazem parte da configuracao

- `solo_1` a `solo_6`: sensores analogicos nos pinos definidos em `BoardPins.h`, calibrados entre molhado e seco.
- `temperatura_interna`: usa o sensor interno do ESP32 e nao possui GPIO externo.
- `bateria`: usa `VBAT_READ`, atualmente GPIO 1, com divisor resistivo de 2:1. Pode ser desabilitada por `USE_BATTERY`.
- `umidade_ar`: reservado para um driver externo, como DHT ou SHT; permanece desabilitado ate o driver e o hardware serem configurados.
- `nivel_reservatorio`: reservado para sensor analogico, boia, ultrassonico ou outro driver; permanece desabilitado ate a configuracao fisica existir.

### Adicionando outro sensor analogico

1. Escolha um `id` unico.
2. Confirme o GPIO em `BoardPins.h`.
3. Adicione uma entrada em `SENSOR_CONFIG`.
4. Use `enabled = true` somente depois de confirmar a ligacao fisica.
5. Defina os pontos de calibracao medidos para aquele sensor.
6. Compile o ambiente `debug` e confira o JSON no monitor serial.

Exemplo de um novo sensor de solo:

```cpp
{"solo_7", SensorType::SoilMoisture, SensorSource::AnalogPin,
 true, 8, 1200, 3500}
```

O registro e a telemetria usam essa configuracao para incluir o sensor; nao e necessario criar um novo campo JSON. Para um tipo de sensor ainda inexistente, crie uma classe que implemente `ISensor`, adicione o caso correspondente em `SensorFactory` e depois inclua a entrada em `SensorConfig.h`. A factory valida IDs e conflitos de GPIO antes de registrar os sensores.

### Calibracao

A calibracao deve ser feita por sensor e documentada junto da configuracao:

- umidade do solo: valor medido no solo molhado e no solo seco;
- nivel de reservatorio: valor correspondente a vazio e cheio;
- bateria: referencia do divisor resistivo e tensao esperada;
- temperatura interna: calibracao `native` do ESP32;
- umidade do ar: valor fornecido pelo driver do sensor.

Nao reutilize pontos de calibracao de sensores diferentes sem medir o hardware real.

## Formato da telemetria

O payload usa um unico array `sensores`. Cada sensor configurado possui nome, tipo, estado, valor bruto, valor calibrado, unidade e calibracao:

```json
{
  "dispositivo": "aabeck-01",
  "tipo": "ESP32V3",
  "versao": "0.0.3-alpha",
  "protocolo_telemetria": 1,
  "timestamp": 123456,
  "sensores": [
    {"id": "solo_1", "tipo": "umidade_solo", "estado": "ok", "valor_raw": 2048, "valor_calibrado": 50.0, "unidade": "percent", "calibracao": "linear_wet_dry"},
    {"id": "temperatura_interna", "tipo": "temperatura_interna", "estado": "ok", "valor_raw": 28, "valor_calibrado": 28.0, "unidade": "celsius", "calibracao": "native"},
    {"id": "bateria", "tipo": "bateria", "estado": "indisponivel", "valor_raw": 0, "valor_calibrado": 0.0, "unidade": "volt", "calibracao": "divider_2_to_1"}
  ]
}
```

O intervalo de envio e configurado por `TEMPO_ENVIO` em `AppConfig.h`; as mensagens de status usam esse mesmo valor.

## Atuadores

`Pump` e `ActuatorManager` fornecem a camada segura para futuras bombas: o GPIO e desligado no boot, existe tempo maximo ligado, cooldown entre partidas, desligamento automatico por timeout e parada de emergencia. Nenhuma bomba e registrada por padrao; um pino e uma politica de seguranca devem ser definidos antes da ativacao fisica.

### Processo para adicionar uma bomba

Uma bomba nao deve ser adicionada apenas como um novo comando. O processo minimo e:

1. Defina um `id` unico para a bomba, por exemplo `bomba_canteiro_1`.
2. Escolha um GPIO de controle que nao esteja sendo usado por display, radio, Serial2 ou sensor.
3. Confirme se o rele ou driver possui logica ativa em nivel alto ou baixo.
4. Instancie `Pump` com GPIO, tempo maximo ligado e cooldown.
5. Registre a bomba no `ActuatorManager`.
6. Confirme que `begin()` inicia o GPIO desligado.
7. Associe protecao contra reservatorio vazio, se disponivel.
8. Adicione comandos de ligar/desligar somente depois que a politica de seguranca estiver definida.
9. Teste primeiro sem carga, depois com o rele, e somente por ultimo com a bomba conectada.

A configuracao deve ser criada em `include/actuators/ActuatorConfig.h`:

```cpp
{"bomba_canteiro_1", ActuatorType::Pump, false, 26, true, 120000, 5000}
```

Os campos sao, respectivamente, identificador, tipo, habilitacao, GPIO,
logica ativa, tempo maximo ligado e cooldown entre partidas. O servico
registra somente atuadores com `enabled = true` e rejeita IDs ou GPIOs
duplicados. Para um novo tipo de atuador, crie a implementacao de
`IActuator` e adicione o caso correspondente em `ActuatorFactory`.

Exemplo estrutural:

```cpp
Pump bomba1("bomba_canteiro_1", GPIO_BOMBA_1, 120000, 5000, true);
ActuatorManager atuadores;

atuadores.add(bomba1);
atuadores.beginAll();
```

O `update(millis())` deve ser chamado continuamente pelo controlador. Quando o tempo maximo for atingido, a bomba sera desligada automaticamente. Antes de ativar uma bomba real, ainda devem existir intertravamento entre bombas, comando `ALL_OFF`, protecao contra funcionamento a seco e uma estrategia de recuperacao apos reinicializacao.

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
