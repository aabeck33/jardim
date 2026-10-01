#include <sensors/SoilMoistureSensor.h>

namespace {

float clampPercent(float value) {
  if (value < 0.0f) return 0.0f;
  if (value > 100.0f) return 100.0f;
  return value;
}

void appendReading(JsonArray output, const SensorReading& value) {
  JsonObject sensor = output.createNestedObject();
  sensor["id"] = value.id;
  sensor["tipo"] = value.type;
  sensor["estado"] = value.state;
  sensor["valor_raw"] = value.rawValue;
  sensor["valor_calibrado"] = value.calibratedValue;
  sensor["unidade"] = value.unit;
  sensor["calibracao"] = value.calibration;
}

}

SoilMoistureSensor::SoilMoistureSensor(
    const char* id, uint8_t pin, uint16_t wetRaw, uint16_t dryRaw)
    : id_(id), pin_(pin), wetRaw_(wetRaw), dryRaw_(dryRaw) {}

bool SoilMoistureSensor::begin() {
  pinMode(pin_, INPUT);
  valid_ = false;
  return true;
}

bool SoilMoistureSensor::read() {
  rawValue_ = analogRead(pin_);
  valid_ = true;
  return valid_;
}

SensorReading SoilMoistureSensor::reading() const {
  const float range = static_cast<float>(dryRaw_) - wetRaw_;
  const float moisture = range == 0.0f
      ? 0.0f
      : 100.0f - ((static_cast<float>(rawValue_) - wetRaw_) * 100.0f / range);
  SensorReading value;
  value.id = id_;
  value.type = "umidade_solo";
  value.state = valid_ ? "ok" : "read_error";
  value.unit = "percent";
  value.calibration = "linear_wet_dry";
  value.rawValue = rawValue_;
  value.calibratedValue = clampPercent(moisture);
  value.valid = valid_;
  return value;
}

void SoilMoistureSensor::appendTo(JsonArray output) const {
  appendReading(output, reading());
}

const char* SoilMoistureSensor::id() const {
  return id_;
}
