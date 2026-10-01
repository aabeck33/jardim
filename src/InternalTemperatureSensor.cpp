#include <sensors/InternalTemperatureSensor.h>

InternalTemperatureSensor::InternalTemperatureSensor(const char* id) : id_(id) {}

bool InternalTemperatureSensor::begin() {
  valid_ = false;
  return true;
}

bool InternalTemperatureSensor::read() {
  value_ = temperatureRead();
  valid_ = true;
  return valid_;
}

SensorReading InternalTemperatureSensor::reading() const {
  SensorReading value;
  value.id = id_;
  value.type = "temperatura_interna";
  value.state = valid_ ? "ok" : "read_error";
  value.unit = "celsius";
  value.calibration = "native";
  value.rawValue = static_cast<int32_t>(value_);
  value.calibratedValue = value_;
  value.valid = valid_;
  return value;
}

void InternalTemperatureSensor::appendTo(JsonArray output) const {
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

const char* InternalTemperatureSensor::id() const {
  return id_;
}
