#ifndef JARDIM_TELEMETRY_SERVICE_H
#define JARDIM_TELEMETRY_SERVICE_H

#include <Arduino.h>
#include <sensors/BatterySensor.h>
#include <sensors/InternalTemperatureSensor.h>
#include <sensors/SensorConfig.h>
#include <sensors/SensorRegistry.h>
#include <sensors/SoilMoistureSensor.h>

class TelemetryService {
public:
  TelemetryService();
  bool begin();
  String collect();
  const SensorRegistry& registry() const;

private:
  SensorRegistry registry_;
  SoilMoistureSensor soil1_;
  SoilMoistureSensor soil2_;
  SoilMoistureSensor soil3_;
  SoilMoistureSensor soil4_;
  SoilMoistureSensor soil5_;
  SoilMoistureSensor soil6_;
  InternalTemperatureSensor temperature_;
  BatterySensor battery_;
  bool initialized_ = false;
};

#endif
