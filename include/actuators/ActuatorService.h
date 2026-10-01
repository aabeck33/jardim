#ifndef JARDIM_ACTUATOR_SERVICE_H
#define JARDIM_ACTUATOR_SERVICE_H

#include <actuators/ActuatorConfig.h>
#include <actuators/ActuatorFactory.h>
#include <actuators/ActuatorManager.h>

class ActuatorService {
public:
  ActuatorService() = default;
  bool begin();
  void update(uint32_t nowMs);
  void emergencyStop();
  StatusCode setState(const char* id, bool active);
  const ActuatorManager& registry() const;

private:
  ActuatorManager registry_;
  ActuatorFactory factory_;
  bool initialized_ = false;
};

#endif
