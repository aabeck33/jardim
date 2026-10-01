#ifndef JARDIM_AIR_HUMIDITY_SENSOR_H
#define JARDIM_AIR_HUMIDITY_SENSOR_H

#include <Arduino.h>
#include <interfaces/ISensor.h>

using HumidityReader = bool (*)(float& humidityPercent);

class AirHumiditySensor : public ISensor {
public:
  AirHumiditySensor(const char* id, HumidityReader reader);
  bool begin() override;
  bool read() override;
  SensorReading reading() const override;
  void appendTo(JsonArray output) const override;
  const char* id() const override;

private:
  const char* id_;
  HumidityReader reader_;
  float humidityPercent_ = 0.0f;
  bool valid_ = false;
};

#endif
