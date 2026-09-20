# Jardim Project Instructions

This project is a PlatformIO-based embedded system using the **ESP32** (Heltec WiFi LoRa 32 V3) and the **Arduino framework**.

## Key Components
- **LoRa Communication**: Uses RadioLib and EByte LoRa E220 libraries.
- **Display**: Adafruit SSD1306 and GFX libraries for OLED display.
- **Data Handling**: ArduinoJson for structured data.
- **Project Structure**:
  - `src/`: Main source code (C++).
  - `lib/`: Local libraries.
  - `include/`: Header files.
  - `platformio.ini`: Project configuration.
  - `central/`: Main source for the central control and data processing module.

## Environment Paths (Auto-detected)
- **Framework Core**: `C:/Users/beck_/.platformio/packages/framework-arduinoespressif32/cores/esp32/`
- **Compiler**: `C:/Users/beck_/.platformio/packages/toolchain-xtensa-esp32s3/bin/xtensa-esp32s3-elf-gcc.exe`
- **Library Dependencies (Local)**: `.pio/libdeps/heltec_wifi_lora_32_v3/`
- **PlatformIO Packages**: `C:/Users/beck_/.platformio/packages/`

## Agent Role
Assist in developing, debugging, and optimizing the firmware and data processing logic for this irrigation/sensing system and its central control module. Whenever a header like `Arduino.h` or `RadioLib.h` is needed, I will consult the paths above to provide accurate contextual help.
