#include <sensors/ReservoirLevelSensor.h>

namespace {

float levelPercent(uint16_t raw, uint16_t emptyRaw, uint16_t fullRaw) {
  const float range = static_cast<float>(fullRaw) - emptyRaw;
  if (range == 0.0f) return 0.0f;
  float value = (static_cast<float>(raw) - emptyRaw) * 100.0f / range;
  if (value < 0.0f) return 0.0f;
  if (value > 100.0f) return 100.0f;
  return value;
}

}

ReservoirLevelSensor::ReservoirLevelSensor(
    const char* id, uint8_t pin, uint16_t emptyRaw, uint16_t fullRaw)
    : id_(id), pin_(pin), emptyRaw_(emptyRaw), fullRaw_(fullRaw) {}

bool ReservoirLevelSensor::begin() {
  pinMode(pin_, INPUT);
  valid_ = false;
  return true;
}

bool ReservoirLevelSensor::read() {
  rawValue_ = analogRead(pin_);
  valid_ = true;
  return true;
}

SensorReading ReservoirLevelSensor::reading() const {
  SensorReading value;
  value.id = id_;
  value.type = "nivel_reservatorio";
  value.state = valid_ ? "ok" : "read_error";
  value.unit = "percent";
  value.calibration = "linear_empty_full";
  value.rawValue = rawValue_;
  value.calibratedValue = levelPercent(rawValue_, emptyRaw_, fullRaw_);
  value.valid = valid_;
  return value;
}

void ReservoirLevelSensor::appendTo(JsonArray output) const {
  JsonObject sensor = output.createNestedObject();
  const SensorReading value = reading();
  sensor["id"] = value.id;
  sensor["tipo"] = value.type;
  sensor["estado"] = value.state;
  sensor["valor_raw"] = value.rawValue;
  sensor["valor_calibrado"] = value.calibratedValue;
  sensor["unidade"] = value.unit;
  sensor["calibracao"] = value.calibration;
}

const char* ReservoirLevelSensor::id() const {
  return id_;
}
