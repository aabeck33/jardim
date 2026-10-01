#ifndef JARDIM_ISENSOR_H
#define JARDIM_ISENSOR_H

#include <ArduinoJson.h>
#include <sensors/SensorReading.h>

class ISensor {
public:
  virtual ~ISensor() = default;
  virtual bool begin() = 0;
  virtual bool read() = 0;
  virtual SensorReading reading() const = 0;
  virtual void appendTo(JsonArray output) const = 0;
  virtual const char* id() const = 0;
};

#endif
