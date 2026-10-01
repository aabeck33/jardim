#ifndef JARDIM_STATUS_CODE_H
#define JARDIM_STATUS_CODE_H

#include <stdint.h>

enum class StatusCode : uint8_t {
  Ok = 0,
  InvalidArgument,
  Timeout,
  HardwareFailure,
  CommunicationFailure,
  NotAvailable,
  NotInitialized,
  UnknownCommand
};

struct OperationResult {
  StatusCode code = StatusCode::Ok;

  bool ok() const {
    return code == StatusCode::Ok;
  }
};

inline const char* statusCodeName(StatusCode code) {
  switch (code) {
    case StatusCode::Ok: return "ok";
    case StatusCode::InvalidArgument: return "invalid_argument";
    case StatusCode::Timeout: return "timeout";
    case StatusCode::HardwareFailure: return "hardware_failure";
    case StatusCode::CommunicationFailure: return "communication_failure";
    case StatusCode::NotAvailable: return "not_available";
    case StatusCode::NotInitialized: return "not_initialized";
    case StatusCode::UnknownCommand: return "unknown_command";
    default: return "unknown";
  }
}

#endif
