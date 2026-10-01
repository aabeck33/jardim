#include <sensors/BatterySensor.h>

BatterySensor::BatterySensor(const char* id, uint8_t pin, bool enabled,
               uint16_t emptyRaw, uint16_t fullRaw)
  : id_(id), pin_(pin), enabled_(enabled), emptyRaw_(emptyRaw), fullRaw_(fullRaw) {}

bool BatterySensor::begin() {
  if (enabled_) {
    pinMode(pin_, INPUT);
  }
  valid_ = false;
  return true;
}

bool BatterySensor::read() {
  if (!enabled_) {
    valid_ = false;
    return false;
  }
  rawValue_ = analogRead(pin_);
  const float normalized = fullRaw_ == emptyRaw_
      ? 0.0f
      : (rawValue_ - emptyRaw_) / static_cast<float>(fullRaw_ - emptyRaw_);
  voltage_ = normalized * 3.3f * 2.0f;
  valid_ = true;
  return true;
}

SensorReading BatterySensor::reading() const {
  SensorReading value;
  value.id = id_;
  value.type = "bateria";
  value.state = !enabled_ ? "indisponivel" : (valid_ ? "ok" : "read_error");
  value.unit = "volt";
  value.calibration = "divider_2_to_1";
  value.rawValue = rawValue_;
  value.calibratedValue = voltage_;
  value.valid = valid_;
  return value;
}

void BatterySensor::appendTo(JsonArray output) const {
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

const char* BatterySensor::id() const {
  return id_;
}
