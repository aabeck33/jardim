#include <services/DisplayService.h>
#include <main.h>
#include <utils.h>

void DisplayService::message(const String& text, uint8_t line) const {
  dispmsg(text, line);
}

void DisplayService::power(bool enabled) const {
  displayOnOff(enabled ? "on" : "off");
}
