#include <actuators/ActuatorManager.h>
#include <string.h>

bool ActuatorManager::add(IActuator& actuator) {
  if (count_ >= MAX_ACTUATORS) {
    return false;
  }
  actuators_[count_++] = &actuator;
  return true;
}

bool ActuatorManager::beginAll() {
  bool result = true;
  for (size_t index = 0; index < count_; ++index) {
    result = actuators_[index]->begin() && result;
  }
  return result;
}

void ActuatorManager::update(uint32_t nowMs) {
  for (size_t index = 0; index < count_; ++index) {
    actuators_[index]->update(nowMs);
  }
}

void ActuatorManager::emergencyStop() {
  for (size_t index = 0; index < count_; ++index) {
    actuators_[index]->setState(false);
  }
}

StatusCode ActuatorManager::setState(const char* id, bool active) {
  for (size_t index = 0; index < count_; ++index) {
    IActuator* actuator = actuators_[index];
    if (strcmp(actuator->id(), id) == 0) {
      actuator->setState(active);
      return actuator->lastStatus();
    }
  }
  return StatusCode::NotAvailable;
}
