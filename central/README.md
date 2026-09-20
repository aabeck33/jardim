# Bloco Central - Sistema de Irrigação Inteligente

Este diretório contém o "cérebro" do sistema de irrigação automatizada. Ele é responsável por processar dados de sensores recebidos via LoRa, consultar previsões climáticas em tempo real, utilizar Inteligência Artificial para decidir se a irrigação deve ser ativada e fornecer uma interface interativa via Streamlit e logs estruturados.

---

## 🚀 Funcionalidades

- **Recepção Contínua (LoRa)**: Escuta mensagens de sensores (umidade do solo, etc.) enviadas por nós remotos (ESP32) usando rádio E220.
- **Integração com Clima**: Busca dados meteorológicos detalhados (temperatura, chuva, umidade, vento) via API Stormglass.
- **Decisão Baseada em IA**: Utiliza modelos de Machine Learning (Classificação e Regressão) para prever a necessidade real e o tempo de irrigação.
- **Controle de Atuadores**: Comanda fisicamente a bomba d'água através dos pinos GPIO do Raspberry Pi com simulação automática em ambiente Desktop (Windows/Mac).
- **Dashboard Web (Streamlit)**: Painel interativo para monitoramento visual em tempo real e análise de métricas.
- **Logs Estruturados**: Gravação contínua de eventos do sistema em arquivo (`logs/app.log`) e console.

---

## 📋 Bibliotecas Utilizadas

- `pyserial`: Comunicação serial com o módulo LoRa E220.
- `RPi.GPIO`: Controle dos pinos físicos do Raspberry Pi (com suporte a fallback simulado em ambiente desktop).
- `scikit-learn`: Execução e treinamento dos modelos de Inteligência Artificial (`RandomForest` / `SGD`).
- `pandas`: Manipulação de dados para o modelo de decisão e exibição em tabelas no Streamlit.
- `joblib`: Serialização e carregamento dos modelos treinados (`.pkl`).
- `requests`: Chamadas para a API de clima.
- `arrow` & `pytz`: Gerenciamento de fusos horários e timestamps.
- `streamlit`: Interface gráfica web para monitoramento interativo.

---

## 🏗️ Arquitetura Completa de Diretórios e Arquivos

```text
central/
│
├── config/                         # Configurações centralizadas e de ambiente
│   ├── __init__.py                 # Reexporta as configurações globais
│   └── settings.py                 # Pinos GPIO, chaves de API, portas seriais e parâmetros LoRa E220
│
├── core/                           # Orquestração e loop principal da aplicação
│   ├── __init__.py                 # Reexporta o IrrigationEngine
│   └── engine.py                   # Classe IrrigationEngine que integra LoRa, Clima, IA e Bomba
│
├── hardware/                       # Camada de abstração de periféricos e comunicação física
│   ├── __init__.py                 # Reexporta inicializadores e acionadores de GPIO
│   ├── gpio_controller.py          # Controle dos pinos da bomba d'água com fallback simulado para Windows
│   └── lora/                       # Módulo de rádio frequência LoRa E220
│       ├── __init__.py             # Interface unificada do módulo LoRa
│       ├── lora_ctrl.py            # Configuração de registradores (ADDH, ADDL, Canal, Potência)
│       ├── lora_receiver.py        # Receiver contínuo multi-thread de mensagens do sensor
│       └── lora_txrx.py            # Funções de envio (TX) e recebimento direto (RX)
│
├── services/                       # Integradores de APIs externas
│   ├── __init__.py                 # Reexporta get_weather
│   └── weather_service.py          # Consulta meteorológica na API Stormglass com fallback
│
├── ia/                             # Inteligência Artificial e Machine Learning
│   ├── __init__.py                 # Reexporta funções de inferência e treino
│   ├── decision_engine.py          # Motor de decisão (decide_irrigation) que consome os modelos ML
│   ├── trainer.py                  # Treino inicial (RandomForest) e contínuo (SGD)
│   ├── models/                     # Artefatos serializados (.pkl)
│   │   ├── modelo_class.pkl        # Modelo de classificação (Irrigar Sim/Não)
│   │   └── modelo_reg.pkl          # Modelo de regressão (Tempo de irrigação em segundos)
│   └── data/                       # Datasets e históricos
│       └── dados_irrigacao.csv     # Dataset base para treinamento dos modelos
│
├── pages/                          # Interface Web / Dashboard Interativo (Streamlit)
│   ├── __init__.py                 # Módulo do dashboard
│   └── app.py                      # Painel interativo Streamlit (`streamlit run pages/app.py`)
│
├── logs/                           # Armazenamento de logs do sistema
│   ├── .gitkeep                    # Mantém o diretório no controle de versão
│   └── app.log                     # Log gerado em tempo de execução com timestamps e níveis de evento
│
├── utils/                          # Funções auxiliares e serviços genéricos
│   ├── __init__.py                 # Reexporta utilitários e logger
│   ├── helpers.py                  # Tratamento e parse seguro de dados JSON
│   └── logger.py                   # Configuração de logging estruturado (console + arquivo)
│
├── tests/                          # Suíte de testes automatizados e validações
│   ├── test_ia.py                  # Teste da API de clima e inferência de IA
│   ├── test_lora_import.py         # Teste de carregamento do driver LoRa
│   └── test_none_fix.py            # Teste de resiliência a dados parciais/ausentes
│
├── main.py                         # Ponto de entrada principal da aplicação backend/core
├── requirements.txt                # Dependências Python do projeto
└── README.md                       # Documentação completa da arquitetura do projeto
```

---

## 🔍 Descrição Detalhada dos Módulos

### 1. Configurações (`config/settings.py`)

Centraliza variáveis globais, pinos de GPIO, parâmetros da API Stormglass e configurações seriais/LoRa E220 (`PIN_BOMBA`, `PIN_M0`, `PIN_M1`, `PIN_AUX`, `STORMGLASS_API_KEY`, etc.).

### 2. Orquestrador Core (`core/engine.py`)

A classe `IrrigationEngine` executa o loop principal de monitoramento: escuta mensagens via LoRa, consulta a previsão do tempo no serviço de clima, solicita a inferência ao motor de IA e aciona a bomba d'água via GPIO.

### 3. Hardware (`hardware/`)

- `gpio_controller.py`: Abstração para acionar a bomba d'água (`aciona_bomba`) e inicializar os GPIOs com suporte automático a simulação em desktops (onde o pacote `RPi.GPIO` não existe).
- `hardware/lora/`: Driver para módulos LoRa E220-900M30S:
  - `lora_receiver.py`: `LoRaRcvCont` cria uma thread dedicada em segundo plano para capturar pacotes de sensores via porta serial (`/dev/serial0`).
  - `lora_ctrl.py`: Permite ler e alterar parâmetros operacionais (endereço, taxa de transmissão, canal e potência).

### 4. Serviços (`services/weather_service.py`)

- `get_weather()`: Consulta a API Stormglass e retorna métricas climáticas. Possui um mecanismo de fallback com valores neutros para evitar paradas do sistema em caso de falhas na API.

### 5. Inteligência Artificial (`ia/`)

- `decision_engine.py`: Função `decide_irrigation()` que consome dados do sensor e do clima através de dois modelos serializados:
  1. **Classificação (`ia/models/modelo_class.pkl`)**: Determina se deve irrigar (binário: 0 ou 1).
  2. **Regressão (`ia/models/modelo_reg.pkl`)**: Determina o tempo ideal em segundos (ex: 15s).
- `trainer.py`: Concentra as rotinas de treinamento inicial (`RandomForest`) e treinamento contínuo (`SGDClassifier`/`SGDRegressor`) salvando os artefatos em `ia/models/`.

### 6. Dashboard Streamlit (`pages/app.py`)

Interface gráfica web moderna desenvolvida em Streamlit para visualização de métricas (status da bomba, umidade atual, chuva prevista) e exibição do histórico de dados.

- **Execução**: `streamlit run pages/app.py`

### 7. Logs do Sistema (`logs/app.log` & `utils/logger.py`)

O módulo `utils/logger.py` gera logs formatados gravados simultaneamente no console e no arquivo `logs/app.log`, permitindo rastrear o histórico de acionamentos e diagnóstico de erros.

---

## 🏃 Como Executar

### Executar a Aplicação Backend (Core Engine):

```bash
python main.py
```

### Executar o Dashboard Web (Streamlit):

```bash
streamlit run pages/app.py
```

### Executar a Suíte de Testes:

```bash
python tests/test_none_fix.py
```

### Teste de serial:

```bash
python3 -m serial.tools.miniterm /dev/serial0 9600
```


---
*Autor: Alvaro Adriano Beck*  
*Versão: 2.0 (Arquitetura Modular com Suporte a Streamlit e Logs)*
