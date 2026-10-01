#ifndef JARDIM_SENSOR_FACTORY_H
#define JARDIM_SENSOR_FACTORY_H

#include <sensors/BatterySensor.h>
#include <sensors/InternalTemperatureSensor.h>
#include <sensors/SensorConfig.h>
#include <sensors/SensorRegistry.h>
#include <sensors/SoilMoistureSensor.h>

class SensorFactory {
public:
  static constexpr size_t MAX_SOIL_SENSORS = 6;

  bool createAll(const SensorConfig* configs, size_t count, SensorRegistry& registry);

private:
  SoilMoistureSensor soilSensors_[MAX_SOIL_SENSORS];
  InternalTemperatureSensor internalTemperature_;
  BatterySensor battery_;
  size_t soilCount_ = 0;
  bool created_ = false;
};

#endif
