#ifndef JARDIM_DISPLAY_SERVICE_H
#define JARDIM_DISPLAY_SERVICE_H

#include <Arduino.h>

class DisplayService {
public:
  void message(const String& text, uint8_t line = 0) const;
  void power(bool enabled) const;
};

#endif
