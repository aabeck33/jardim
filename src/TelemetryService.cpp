#include <services/TelemetryService.h>
#include <main.h>
#include <utils.h>

bool TelemetryService::begin() {
  initialized_ = factory_.createAll(
      SENSOR_CONFIG,
      SENSOR_CONFIG_COUNT,
      registry_) && registry_.beginAll();
  return initialized_;
}

String TelemetryService::collect() {
  if (!initialized_) {
    begin();
  }
  registry_.readAll();

  StaticJsonDocument<JSON_DOC_SIZE> data;
  data["dispositivo"] = DISPOSITIVO;
  data["tipo"] = TIPO_DISPOSITIVO;
  data["versao"] = VERSAO_FIRMWARE;
  data["protocolo_telemetria"] = PROTOCOLO_TELEMETRIA;
  data["timestamp"] = millis();

  JsonArray sensors = data.createNestedArray("sensores");
  registry_.appendTo(sensors);

  dispmsg("Dados coletados.");
  verificarUsoJson(data);

  String payload;
  payload.reserve(JSON_DOC_SIZE);
  serializeJson(data, payload);
  return payload;
}

const SensorRegistry& TelemetryService::registry() const {
  return registry_;
}
