#ifndef JARDIM_BATTERY_SENSOR_H
#define JARDIM_BATTERY_SENSOR_H

#include <Arduino.h>
#include <interfaces/ISensor.h>

class BatterySensor : public ISensor {
public:
  BatterySensor() = default;
  BatterySensor(const char* id, uint8_t pin, bool enabled,
                uint16_t emptyRaw = 0, uint16_t fullRaw = 4095);
  void configure(const char* id, uint8_t pin, bool enabled,
                 uint16_t emptyRaw, uint16_t fullRaw);
  bool begin() override;
  bool read() override;
  SensorReading reading() const override;
  void appendTo(JsonArray output) const override;
  const char* id() const override;

private:
  const char* id_;
  uint8_t pin_;
  bool enabled_;
  uint16_t emptyRaw_;
  uint16_t fullRaw_;
  uint16_t rawValue_ = 0;
  float voltage_ = 0.0f;
  bool valid_ = false;
};

#endif
