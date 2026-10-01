#include <sensors/SensorRegistry.h>
#include <string.h>

bool SensorRegistry::add(ISensor& sensor) {
  if (count_ >= MAX_SENSORS) {
    return false;
  }
  sensors_[count_++] = &sensor;
  return true;
}

bool SensorRegistry::beginAll() {
  bool result = true;
  for (size_t index = 0; index < count_; ++index) {
    result = sensors_[index]->begin() && result;
  }
  return result;
}

bool SensorRegistry::readAll() {
  bool result = true;
  for (size_t index = 0; index < count_; ++index) {
    result = sensors_[index]->read() && result;
  }
  return result;
}

void SensorRegistry::appendTo(JsonArray output, const char* typeFilter) const {
  for (size_t index = 0; index < count_; ++index) {
    const SensorReading value = sensors_[index]->reading();
    if (typeFilter == nullptr || strcmp(value.type, typeFilter) == 0) {
      sensors_[index]->appendTo(output);
    }
  }
}

size_t SensorRegistry::size() const {
  return count_;
}
