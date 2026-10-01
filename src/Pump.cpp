#include <actuators/Pump.h>

Pump::Pump(const char* id, uint8_t pin, uint32_t maxRunTimeMs,
           uint32_t restartCooldownMs, bool activeHigh)
    : id_(id), pin_(pin), maxRunTimeMs_(maxRunTimeMs),
      restartCooldownMs_(restartCooldownMs), activeHigh_(activeHigh) {}

void Pump::configure(const char* id, uint8_t pin, uint32_t maxRunTimeMs,
                     uint32_t restartCooldownMs, bool activeHigh) {
  id_ = id;
  pin_ = pin;
  maxRunTimeMs_ = maxRunTimeMs;
  restartCooldownMs_ = restartCooldownMs;
  activeHigh_ = activeHigh;
  active_ = false;
  lastStatus_ = StatusCode::NotInitialized;
}

bool Pump::begin() {
  pinMode(pin_, OUTPUT);
  active_ = false;
  stoppedAtMs_ = millis();
  writeHardware(false);
  lastStatus_ = StatusCode::Ok;
  return true;
}

bool Pump::setState(bool active) {
  const uint32_t nowMs = millis();
  if (active == active_) {
    lastStatus_ = StatusCode::Ok;
    return true;
  }

  if (active && nowMs - stoppedAtMs_ < restartCooldownMs_) {
    lastStatus_ = StatusCode::NotAvailable;
    return false;
  }

  active_ = active;
  if (active_) {
    startedAtMs_ = nowMs;
  } else {
    stoppedAtMs_ = nowMs;
  }
  writeHardware(active_);
  lastStatus_ = StatusCode::Ok;
  return true;
}

void Pump::update(uint32_t nowMs) {
  if (active_ && nowMs - startedAtMs_ >= maxRunTimeMs_) {
    active_ = false;
    stoppedAtMs_ = nowMs;
    writeHardware(false);
    lastStatus_ = StatusCode::Timeout;
  }
}

bool Pump::isActive() const {
  return active_;
}

StatusCode Pump::lastStatus() const {
  return lastStatus_;
}

const char* Pump::id() const {
  return id_;
}

void Pump::writeHardware(bool active) {
  const uint8_t onLevel = activeHigh_ ? HIGH : LOW;
  const uint8_t offLevel = activeHigh_ ? LOW : HIGH;
  digitalWrite(pin_, active ? onLevel : offLevel);
}
