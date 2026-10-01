#ifndef JARDIM_ISENSOR_H
#define JARDIM_ISENSOR_H

#include <ArduinoJson.h>
#include <stdint.h>

class ISensor {
public:
  virtual ~ISensor() = default;
  virtual bool begin() = 0;
  virtual bool read() = 0;
  virtual void appendTo(JsonObject output) const = 0;
  virtual const char* id() const = 0;
};

#endif
