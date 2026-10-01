#ifndef JARDIM_ACTUATOR_CONFIG_H
#define JARDIM_ACTUATOR_CONFIG_H

#include <BoardPins.h>
#include <stdint.h>

enum class ActuatorType : uint8_t {
  Pump,
  Valve
};

struct ActuatorConfig {
  const char* id;
  ActuatorType type;
  bool enabled;
  uint8_t pin;
  bool activeHigh;
  uint32_t maxRunTimeMs;
  uint32_t restartCooldownMs;
};

constexpr ActuatorConfig ACTUATOR_CONFIG[] = {
  {"bomba_canteiro_1", ActuatorType::Pump, false, 26, true, 120000, 5000}
};

constexpr size_t ACTUATOR_CONFIG_COUNT =
    sizeof(ACTUATOR_CONFIG) / sizeof(ACTUATOR_CONFIG[0]);

#endif
