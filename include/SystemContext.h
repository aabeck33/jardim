#ifndef JARDIM_SYSTEM_CONTEXT_H
#define JARDIM_SYSTEM_CONTEXT_H

#include <Arduino.h>

struct SystemContext {
  bool safeMode = false;
  bool displayReady = false;
  bool serialReady = false;
  uint32_t lastMessageMillis = 0;
  uint32_t telemetryCount = 0;
};

#endif
