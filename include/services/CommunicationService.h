#ifndef JARDIM_COMMUNICATION_SERVICE_H
#define JARDIM_COMMUNICATION_SERVICE_H

#include <Arduino.h>

class CommunicationService {
public:
  void sendTelemetry(const String& payload) const;
  void pollCommands() const;
};

#endif
