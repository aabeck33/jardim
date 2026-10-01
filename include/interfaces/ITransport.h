#ifndef JARDIM_ITRANSPORT_H
#define JARDIM_ITRANSPORT_H

#include <stddef.h>
#include <stdint.h>

class ITransport {
public:
  virtual ~ITransport() = default;
  virtual bool begin() = 0;
  virtual bool send(const uint8_t* data, size_t length) = 0;
  virtual void poll() = 0;
};

#endif
