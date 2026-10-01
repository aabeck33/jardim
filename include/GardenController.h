#ifndef JARDIM_GARDEN_CONTROLLER_H
#define JARDIM_GARDEN_CONTROLLER_H

#include <services/CommunicationService.h>
#include <services/DisplayService.h>
#include <services/TelemetryService.h>

class GardenController {
public:
  void begin();
  void update();

private:
  bool loraExtReady_ = false;
  CommunicationService communication_;
  DisplayService display_;
  TelemetryService telemetry_;
};

#endif
