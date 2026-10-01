#ifndef JARDIM_ACTUATOR_MANAGER_H
#define JARDIM_ACTUATOR_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include <StatusCode.h>
#include <interfaces/IActuator.h>

class ActuatorManager {
public:
  static constexpr size_t MAX_ACTUATORS = 4;

  bool add(IActuator& actuator);
  bool beginAll();
  void update(uint32_t nowMs);
  void emergencyStop();
  StatusCode setState(const char* id, bool active);

private:
  IActuator* actuators_[MAX_ACTUATORS] = {};
  size_t count_ = 0;
};

#endif
