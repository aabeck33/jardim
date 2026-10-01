#ifndef JARDIM_SENSOR_READING_H
#define JARDIM_SENSOR_READING_H

#include <stdint.h>

struct SensorReading {
  const char* id = "";
  const char* type = "";
  const char* state = "indisponivel";
  const char* unit = "";
  const char* calibration = "";
  int32_t rawValue = 0;
  float calibratedValue = 0.0f;
  bool valid = false;
};

#endif
