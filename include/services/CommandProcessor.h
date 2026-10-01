#ifndef JARDIM_COMMAND_PROCESSOR_H
#define JARDIM_COMMAND_PROCESSOR_H

#include <Arduino.h>
#include <StatusCode.h>

using CommandHandler = StatusCode (*)(const String& arguments);

class CommandProcessor {
public:
  CommandProcessor();
  StatusCode registerHandler(const char* name, CommandHandler handler);
  StatusCode process(const String& command) const;

private:
  static constexpr size_t MAX_HANDLERS = 8;
  struct Entry {
    const char* name = nullptr;
    CommandHandler handler = nullptr;
  };

  Entry handlers_[MAX_HANDLERS];
  size_t count_ = 0;
};

#endif
