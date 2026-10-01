#ifndef JARDIM_IACTUATOR_H
#define JARDIM_IACTUATOR_H

#include <stdint.h>

class IActuator {
public:
  virtual ~IActuator() = default;
  virtual bool begin() = 0;
  virtual bool setState(bool active) = 0;
  virtual void update(uint32_t nowMs) = 0;
  virtual bool isActive() const = 0;
};

#endif
