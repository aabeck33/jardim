#ifndef JARDIM_TELEMETRY_SERVICE_H
#define JARDIM_TELEMETRY_SERVICE_H

#include <Arduino.h>

class TelemetryService {
public:
  String collect() const;
};

#endif
