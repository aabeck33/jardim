# Projeto Jardim Inteligente 🌿

Este projeto consiste em um sistema de irrigação automatizado e inteligente, composto por nós sensores remotos (Arduino/ESP32) e uma central de controle (Raspberry Pi) com Inteligência Artificial.

---

## 📡 Programação Arduino (Nós Sensores)

Os nós remotos são responsáveis pela coleta de dados em campo e transmissão via rádio para a central.

### Hardware Base
- **Placa**: Heltec WiFi LoRa 32 V3 (ESP32-S3).
- **Rádio**: SX1262 (Integrado) ou EByte E220 (Externo).
- **Sensores**: Umidade do solo, Tensão da bateria e Temperatura interna.
- **Display**: OLED SSD1306 128x64 (I2C).

### Funcionalidades do Firmware
- **Coleta de Dados**: Leitura analógica dos sensores de umidade e monitoramento de bateria.
- **Formatação JSON**: Organiza os dados em objetos JSON para facilitar o processamento na central.
- **Segurança**: Criptografia XOR simples para proteção dos dados transmitidos.
- **Economia de Energia**: Uso de *Deep Sleep* (sono profundo) para operar por meses com bateria.
- **Watchdog**: Temporizador de segurança para evitar travamentos do sistema.
- **Modo Seguro**: Inicialização simplificada para recuperação e testes.

### Tecnologias e Dependências (ESP32)
- **Plataforma**: `espressif32 @ 6.13.0` (Framework Arduino `v3.20017`)
- **Bibliotecas Principais**:
  - `RadioLib @ 7.6.0`: Comunicação avançada com o chip LoRa SX1262.
  - `EByte LoRa E220 library @ 1.0.8`: Controle do módulo externo E220.
  - `ArduinoJson @ 6.21.6`: Serialização eficiente dos dados dos sensores.
  - `Adafruit SSD1306 @ 2.5.16` & `GFX @ 1.12.5`: Interface visual no display OLED.
  - `Adafruit BusIO @ 1.17.4`: Gerenciamento de barramentos I2C/SPI.

---

## 🧠 Bloco Central (Controller)

A central processa as informações e toma as decisões de irrigação. Para detalhes técnicos sobre a Inteligência Artificial e a API de Clima, consulte o [README da pasta central](./central/README.md).

### Resumo das Funções
- **Receptor LoRa**: Thread dedicada para escuta contínua dos nós.
- **Decisão por IA**: Modelos de Machine Learning (Scikit-Learn) que analisam umidade + previsão do tempo.
- **API StormGlass**: Fonte oficial para dados climáticos em tempo real.

---

## 🛠️ Como Compilar

### Firmware (ESP32)
1. Instale o VS Code com a extensão **PlatformIO**.
2. Abra a pasta raiz do projeto.
3. Configure os pinos e chaves no arquivo `src/setup.h`.
4. Clique em **Build** e **Upload**.

### Central (Raspberry Pi)
1. Certifique-se de ter o Python 3.10+ instalado.
2. Instale as dependências: `pip install -r central/requirements.txt` (ou manualmente: pandas, scikit-learn, joblib, pyserial).
3. Execute: `python central/main.py`.

---

## 🚀 Melhorias Futuras
- **Criptografia Avançada**: Evoluir o esquema XOR simples para chaves longas ou implementação de hardware **AES-128** no ESP32 para proteção robusta dos dados.
- **Otimização de Memória**: Refatorar o firmware para substituir a classe `String` por buffers de caracteres fixos (`char[]`), prevenindo fragmentação de memória e aumentando a disponibilidade do sistema.
- **Gerenciamento de Energia**: Otimizar ciclos de despertar e monitoramento detalhado de nível de bateria.

---
*Desenvolvido por: Alvaro Adriano Beck*
