#ifndef JARDIM_SENSOR_CONFIG_H
#define JARDIM_SENSOR_CONFIG_H

#include <AppConfig.h>
#include <BoardPins.h>
#include <stdint.h>

constexpr uint8_t SENSOR_NO_PIN = 0xFF;

enum class SensorType : uint8_t {
  SoilMoisture,
  InternalTemperature,
  Battery,
  AirHumidity,
  ReservoirLevel
};

enum class SensorSource : uint8_t {
  AnalogPin,
  InternalEsp32,
  ExternalDriver
};

struct SensorConfig {
  const char* id;
  SensorType type;
  SensorSource source;
  bool enabled;
  bool available;
  uint8_t pin;
  uint16_t firstCalibrationPoint;
  uint16_t secondCalibrationPoint;
};

constexpr SensorConfig SENSOR_CONFIG[] = {
  {"solo_1", SensorType::SoilMoisture, SensorSource::AnalogPin, true, true, pinosEntrada[0], 0, 4095},
  {"solo_2", SensorType::SoilMoisture, SensorSource::AnalogPin, true, true, pinosEntrada[1], 0, 4095},
  {"solo_3", SensorType::SoilMoisture, SensorSource::AnalogPin, true, true, pinosEntrada[2], 0, 4095},
  {"solo_4", SensorType::SoilMoisture, SensorSource::AnalogPin, true, true, pinosEntrada[3], 0, 4095},
  {"solo_5", SensorType::SoilMoisture, SensorSource::AnalogPin, true, true, pinosEntrada[4], 0, 4095},
  {"solo_6", SensorType::SoilMoisture, SensorSource::AnalogPin, true, true, pinosEntrada[5], 0, 4095},
  {"temperatura_interna", SensorType::InternalTemperature, SensorSource::InternalEsp32, true, true, SENSOR_NO_PIN, 0, 0},
  {"bateria", SensorType::Battery, SensorSource::AnalogPin, true, USE_BATTERY, VBAT_READ, 0, 4095},
  {"umidade_ar", SensorType::AirHumidity, SensorSource::ExternalDriver, false, false, SENSOR_NO_PIN, 0, 100},
  {"nivel_reservatorio", SensorType::ReservoirLevel, SensorSource::AnalogPin, false, false, SENSOR_NO_PIN, 0, 4095}
};

constexpr size_t SENSOR_CONFIG_COUNT = sizeof(SENSOR_CONFIG) / sizeof(SENSOR_CONFIG[0]);

#endif
