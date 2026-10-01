#include <actuators/ActuatorService.h>

bool ActuatorService::begin() {
  initialized_ = factory_.createAll(
      ACTUATOR_CONFIG,
      ACTUATOR_CONFIG_COUNT,
      registry_) && registry_.beginAll();
  return initialized_;
}

void ActuatorService::update(uint32_t nowMs) {
  registry_.update(nowMs);
}

void ActuatorService::emergencyStop() {
  registry_.emergencyStop();
}

StatusCode ActuatorService::setState(const char* id, bool active) {
  return registry_.setState(id, active);
}

const ActuatorManager& ActuatorService::registry() const {
  return registry_;
}
