#ifndef JARDIM_SENSOR_REGISTRY_H
#define JARDIM_SENSOR_REGISTRY_H

#include <ArduinoJson.h>
#include <interfaces/ISensor.h>

class SensorRegistry {
public:
  static constexpr size_t MAX_SENSORS = 12;

  bool add(ISensor& sensor);
  bool beginAll();
  bool readAll();
  void appendTo(JsonArray output, const char* typeFilter = nullptr) const;
  size_t size() const;

private:
  ISensor* sensors_[MAX_SENSORS] = {};
  size_t count_ = 0;
};

#endif
