# Comentarios recuperados de main.h

Este arquivo preserva comentarios e observacoes que existiam na versao anterior de `main.h`. Eles foram separados do codigo para permanecerem como referencia sem reintroduzir configuracoes ou dependencias legadas.

## Configuracao e recursos

As flags de compilacao permitem habilitar ou desabilitar display, LoRa,
Wi-Fi, bateria, Bluetooth, SPIFFS, Serial2, criptografia, deep sleep,
modo de teste e comandos recebidos.

A configuracao da aplicacao deve ficar em `AppConfig.h`.
Os pinos e parametros especificos da placa devem ficar em `BoardPins.h`.
As credenciais locais devem ficar em `secrets.h`.

// Cada caractere no display ocupa 6x8 pixels, então 128/6 = 21 caracteres por linha, 64/8 = 8 linhas

## Entradas analogicas

Um ADC de 12 bits produz valores entre 0 e 4095.

Com referencia de 3.3 V, cada unidade representa aproximadamente:

```text
3.3 V / 4096 = 0.0008 V
```

Entradas analogicas atualmente configuradas:

- GPIO 2
- GPIO 3
- GPIO 4
- GPIO 5
- GPIO 6
- GPIO 7

Esses pinos sao usados pelos sensores analogicos configurados em
`SensorConfig.h`. Cada sensor deve possuir um identificador, tipo, pino,
habilitacao e pontos de calibracao proprios.

## Pinos somente digitais

Pinos identificados como somente digitais na placa:

- GPIO 26
- GPIO 33
- GPIO 34
- GPIO 38
- GPIO 39
- GPIO 40
- GPIO 41
- GPIO 42
- GPIO 45
- GPIO 46
- GPIO 47
- GPIO 48

// Um ADC (Conversor Analógico-Digital) de 12 bits gera valores de 0 a 4095 (2¹² - 1).
// Portanto, se sua tensão de referência for 3.3V, o valor 4095 representa 3.3V, e 0 representa 0V.
// Cada unidade no valor representa cerca de 0.0008V (3.3V ÷ 4096).
Pinos ADC: GPIO 2, 3, 4, 5, 6, 7, 19, 20
Pinos somente digitais: GPIO 33,  34, 38, 39, 40, 42, 42, 45, 46, 47?, 48?

Antes de adicionar um sensor ou atuador, confirmar no esquema da placa
se o GPIO escolhido esta disponivel e se nao esta reservado para display,
radio, Serial2, bateria ou outro periferico.

## Sensores internos e bateria

A temperatura interna usa o sensor interno do ESP32 e nao utiliza GPIO
externo.

A bateria usa o pino `VBAT_READ`, atualmente GPIO 1, por meio de divisor
resistivo. A conversao deve considerar a proporcao real do divisor.

## Comunicacao LoRa

Os parametros de frequencia, potencia e modulacao devem ser iguais nos
dispositivos que se comunicam.

Os parametros LoRa devem respeitar:

- regulamentacao local de frequencia;
- limite de potencia de transmissao;
- fator de espalhamento;
- largura de banda;
- taxa de codificacao;
- tamanho do preambulo;
- palavra de sincronizacao;
- verificacao de CRC.

Alterar esses valores exige validar novamente a comunicacao entre ESP32
e Central.

## Watchdog e modo seguro

O watchdog impede que o firmware permaneça indefinidamente em um loop
travado.

O modo seguro deve impedir a inicializacao completa dos perifericos
quando houver falha critica, permitindo diagnostico e recuperacao.

## Deep sleep

O intervalo de envio deve ser controlado por `TEMPO_ENVIO` em
`AppConfig.h`. Mensagens, temporizadores e logs devem usar essa mesma
configuracao, sem valores fixos escritos no texto.

## Regra para novos modulos

Todo novo sensor ou atuador deve:

1. Possuir um identificador configurado.
2. Declarar explicitamente sua origem e seus pinos.
3. Ter estado de habilitacao.
4. Definir sua unidade e calibracao.
5. Ser registrado no modulo correspondente.
6. Ser testado com o ambiente debug antes de ser ativado em producao.
