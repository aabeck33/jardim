#include <sensors/AirHumiditySensor.h>

AirHumiditySensor::AirHumiditySensor(const char* id, HumidityReader reader)
    : id_(id), reader_(reader) {}

bool AirHumiditySensor::begin() {
  valid_ = false;
  return reader_ != nullptr;
}

bool AirHumiditySensor::read() {
  if (reader_ == nullptr) {
    valid_ = false;
    return false;
  }
  valid_ = reader_(humidityPercent_);
  return valid_;
}

SensorReading AirHumiditySensor::reading() const {
  SensorReading value;
  value.id = id_;
  value.type = "umidade_ar";
  value.state = valid_ ? "ok" : "read_error";
  value.unit = "percent";
  value.calibration = "driver";
  value.rawValue = static_cast<int32_t>(humidityPercent_ * 100.0f);
  value.calibratedValue = humidityPercent_;
  value.valid = valid_;
  return value;
}

void AirHumiditySensor::appendTo(JsonArray output) const {
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

const char* AirHumiditySensor::id() const {
  return id_;
}
