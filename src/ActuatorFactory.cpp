#include <actuators/ActuatorFactory.h>
#include <string.h>

bool ActuatorFactory::createAll(const ActuatorConfig* configs, size_t count,
                                ActuatorManager& manager) {
  if (configs == nullptr) {
    return false;
  }

  for (size_t first = 0; first < count; ++first) {
    if (!configs[first].enabled || configs[first].id == nullptr) {
      continue;
    }
    for (size_t second = first + 1; second < count; ++second) {
      if (!configs[second].enabled || configs[second].id == nullptr) {
        continue;
      }
      if (strcmp(configs[first].id, configs[second].id) == 0 ||
          configs[first].pin == configs[second].pin) {
        return false;
      }
    }
  }

  bool result = true;
  for (size_t index = 0; index < count; ++index) {
    const ActuatorConfig& config = configs[index];
    if (!config.enabled) {
      continue;
    }

    if (config.type != ActuatorType::Pump || pumpCount_ >= MAX_PUMPS) {
      result = false;
      continue;
    }

    pumps_[pumpCount_].configure(
        config.id,
        config.pin,
        config.maxRunTimeMs,
        config.restartCooldownMs,
        config.activeHigh);
    if (!manager.add(pumps_[pumpCount_])) {
      result = false;
    }
    ++pumpCount_;
  }

  return result;
}