#include <main.h>
#include <GardenController.h>

RTC_DATA_ATTR SystemContext systemContext;
Configuration configE220std;

#if (USE_LORA)
SX1262 lora = new Module(
  LORA_NSS,
  LORA_DIO1,
  LORA_RST,
  LORA_BUSY
);
#endif

#if (USE_LORA_EXT)
LoRa_E220 LoRaExt(
  &Serial2,
  LORA_EXT_AUX,
  LORA_EXT_M0,
  LORA_EXT_M1
);
#endif

#if (USE_DISPLAY)
Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);
#endif

GardenController app;

void setup() {
  app.begin();
}

void loop() {
  app.update();
}
