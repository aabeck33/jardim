#include <services/CommandProcessor.h>
#include <main.h>

namespace {

bool hasArguments(const String& arguments) {
  String value = arguments;
  value.trim();
  return value.length() > 0;
}

StatusCode handleLedOn(const String& arguments) {
  if (hasArguments(arguments)) {
    return StatusCode::InvalidArgument;
  }
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ligado");
  return StatusCode::Ok;
}

StatusCode handleLedOff(const String& arguments) {
  if (hasArguments(arguments)) {
    return StatusCode::InvalidArgument;
  }
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED desligado");
  return StatusCode::Ok;
}

StatusCode handleSleep(const String& arguments) {
  String value = arguments;
  value.trim();
  if (value.length() == 0) {
    return StatusCode::InvalidArgument;
  }

  for (size_t index = 0; index < value.length(); ++index) {
    if (!isdigit(value[index])) {
      return StatusCode::InvalidArgument;
    }
  }

  const long seconds = value.toInt();
  if (seconds <= 0 || seconds > 86400) {
    return StatusCode::InvalidArgument;
  }

  Serial.print("Dormir por ");
  Serial.print(seconds);
  Serial.println(" segundos");
  delay(100);
  esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(seconds) * 1000000ULL);
  esp_deep_sleep_start();
  return StatusCode::Ok;
}

}

CommandProcessor::CommandProcessor() {
  registerHandler("LED_ON", handleLedOn);
  registerHandler("LED_OFF", handleLedOff);
  registerHandler("SLEEP", handleSleep);
}

StatusCode CommandProcessor::registerHandler(
    const char* name,
    CommandHandler handler) {
  if (name == nullptr || handler == nullptr || count_ >= MAX_HANDLERS) {
    return StatusCode::InvalidArgument;
  }

  handlers_[count_].name = name;
  handlers_[count_].handler = handler;
  ++count_;
  return StatusCode::Ok;
}

StatusCode CommandProcessor::process(const String& command) const {
  String value = command;
  value.trim();
  if (value.length() == 0) {
    return StatusCode::InvalidArgument;
  }

  const int separator = value.indexOf(' ');
  const String name = separator < 0 ? value : value.substring(0, separator);
  const String arguments = separator < 0 ? String() : value.substring(separator + 1);

  for (size_t index = 0; index < count_; ++index) {
    if (name == handlers_[index].name) {
      return handlers_[index].handler(arguments);
    }
  }

  return StatusCode::UnknownCommand;
}
