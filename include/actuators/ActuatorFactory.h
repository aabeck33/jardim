#ifndef JARDIM_ACTUATOR_FACTORY_H
#define JARDIM_ACTUATOR_FACTORY_H

#include <actuators/ActuatorConfig.h>
#include <actuators/ActuatorManager.h>
#include <actuators/Pump.h>

class ActuatorFactory {
public:
  static constexpr size_t MAX_PUMPS = 4;

  bool createAll(const ActuatorConfig* configs, size_t count,
                 ActuatorManager& manager);

private:
  Pump pumps_[MAX_PUMPS];
  size_t pumpCount_ = 0;
};

#endif