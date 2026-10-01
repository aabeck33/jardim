#ifndef JARDIM_IACTUATOR_H
#define JARDIM_IACTUATOR_H

#include <stdint.h>
#include <StatusCode.h>

class IActuator {
public:
  virtual ~IActuator() = default;
  virtual bool begin() = 0;
  virtual bool setState(bool active) = 0;
  virtual void update(uint32_t nowMs) = 0;
  virtual bool isActive() const = 0;
  virtual StatusCode lastStatus() const = 0;
  virtual const char* id() const = 0;
};

#endif
