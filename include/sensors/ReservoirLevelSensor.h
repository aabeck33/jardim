#ifndef JARDIM_RESERVOIR_LEVEL_SENSOR_H
#define JARDIM_RESERVOIR_LEVEL_SENSOR_H

#include <Arduino.h>
#include <interfaces/ISensor.h>

class ReservoirLevelSensor : public ISensor {
public:
  ReservoirLevelSensor(const char* id, uint8_t pin, uint16_t emptyRaw, uint16_t fullRaw);
  bool begin() override;
  bool read() override;
  SensorReading reading() const override;
  void appendTo(JsonArray output) const override;
  const char* id() const override;

private:
  const char* id_;
  uint8_t pin_;
  uint16_t emptyRaw_;
  uint16_t fullRaw_;
  uint16_t rawValue_ = 0;
  bool valid_ = false;
};

#endif
