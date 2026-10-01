#ifndef JARDIM_INTERNAL_TEMPERATURE_SENSOR_H
#define JARDIM_INTERNAL_TEMPERATURE_SENSOR_H

#include <Arduino.h>
#include <interfaces/ISensor.h>

class InternalTemperatureSensor : public ISensor {
public:
  explicit InternalTemperatureSensor(const char* id = "temperatura");
  bool begin() override;
  bool read() override;
  SensorReading reading() const override;
  void appendTo(JsonArray output) const override;
  const char* id() const override;

private:
  const char* id_;
  float value_ = 0.0f;
  bool valid_ = false;
};

#endif
