#ifndef JARDIM_SOIL_MOISTURE_SENSOR_H
#define JARDIM_SOIL_MOISTURE_SENSOR_H

#include <Arduino.h>
#include <interfaces/ISensor.h>

class SoilMoistureSensor : public ISensor {
public:
  SoilMoistureSensor() = default;
  SoilMoistureSensor(const char* id, uint8_t pin, uint16_t wetRaw = 0, uint16_t dryRaw = 4095);
  void configure(const char* id, uint8_t pin, uint16_t wetRaw, uint16_t dryRaw);
  bool begin() override;
  bool read() override;
  SensorReading reading() const override;
  void appendTo(JsonArray output) const override;
  const char* id() const override;

private:
  const char* id_;
  uint8_t pin_;
  uint16_t wetRaw_;
  uint16_t dryRaw_;
  uint16_t rawValue_ = 0;
  bool valid_ = false;
};

#endif
