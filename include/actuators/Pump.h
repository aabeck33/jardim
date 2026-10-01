#ifndef JARDIM_PUMP_H
#define JARDIM_PUMP_H

#include <Arduino.h>
#include <StatusCode.h>
#include <interfaces/IActuator.h>

class Pump : public IActuator {
public:
  Pump(const char* id, uint8_t pin, uint32_t maxRunTimeMs,
       uint32_t restartCooldownMs = 5000, bool activeHigh = true);

  bool begin() override;
  bool setState(bool active) override;
  void update(uint32_t nowMs) override;
  bool isActive() const override;
  StatusCode lastStatus() const;
  const char* id() const override;

private:
  void writeHardware(bool active);

  const char* id_;
  uint8_t pin_;
  uint32_t maxRunTimeMs_;
  uint32_t restartCooldownMs_;
  bool activeHigh_;
  uint32_t startedAtMs_ = 0;
  uint32_t stoppedAtMs_ = 0;
  bool active_ = false;
  StatusCode lastStatus_ = StatusCode::NotInitialized;
};

#endif
