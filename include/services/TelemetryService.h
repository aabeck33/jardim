#ifndef JARDIM_TELEMETRY_SERVICE_H
#define JARDIM_TELEMETRY_SERVICE_H

#include <Arduino.h>
#include <sensors/SensorConfig.h>
#include <sensors/SensorFactory.h>
#include <sensors/SensorRegistry.h>

class TelemetryService {
public:
  TelemetryService() = default;
  bool begin();
  String collect();
  const SensorRegistry& registry() const;

private:
  SensorRegistry registry_;
  SensorFactory factory_;
  bool initialized_ = false;
};

#endif
