#include <services/CommunicationService.h>
#include <utils.h>

void CommunicationService::sendTelemetry(const String& payload) const {
  enviarDados(payload);
}

void CommunicationService::pollCommands() const {
  receberComandoLoRa();
}
