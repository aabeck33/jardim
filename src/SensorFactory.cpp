#include <sensors/SensorFactory.h>
#include <string.h>

bool SensorFactory::createAll(
    const SensorConfig* configs,
    size_t count,
    SensorRegistry& registry) {
  if (created_ || configs == nullptr) {
    return false;
  }

  for (size_t first = 0; first < count; ++first) {
    if (!configs[first].enabled || configs[first].id == nullptr) {
      continue;
    }
    for (size_t second = first + 1; second < count; ++second) {
      if (!configs[second].enabled || configs[second].id == nullptr) {
        continue;
      }
      if (strcmp(configs[first].id, configs[second].id) == 0 ||
          (configs[first].source == SensorSource::AnalogPin &&
           configs[second].source == SensorSource::AnalogPin &&
           configs[first].pin != SENSOR_NO_PIN &&
           configs[first].pin == configs[second].pin)) {
        return false;
      }
    }
  }

  bool result = true;
  for (size_t index = 0; index < count; ++index) {
    const SensorConfig& config = configs[index];
    if (!config.enabled) {
      continue;
    }

    switch (config.type) {
      case SensorType::SoilMoisture:
        if (soilCount_ >= MAX_SOIL_SENSORS) {
          result = false;
          continue;
        }
        soilSensors_[soilCount_].configure(
            config.id,
            config.pin,
            config.firstCalibrationPoint,
            config.secondCalibrationPoint);
        if (config.available && !registry.add(soilSensors_[soilCount_])) {
          result = false;
        }
        ++soilCount_;
        break;

      case SensorType::InternalTemperature:
        internalTemperature_.configure(config.id);
        if (config.available && !registry.add(internalTemperature_)) {
          result = false;
        }
        break;

      case SensorType::Battery:
        battery_.configure(
            config.id,
            config.pin,
            config.available,
            config.firstCalibrationPoint,
            config.secondCalibrationPoint);
        if (!registry.add(battery_)) {
          result = false;
        }
        break;

      case SensorType::AirHumidity:
      case SensorType::ReservoirLevel:
        result = false;
        break;
    }
  }

  created_ = result;
  return result;
}
