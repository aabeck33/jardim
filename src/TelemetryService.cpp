#include <services/TelemetryService.h>
#include <main.h>
#include <utils.h>

TelemetryService::TelemetryService()
    : soil1_(SENSOR_CONFIG[0].id, SENSOR_CONFIG[0].pin,
        SENSOR_CONFIG[0].firstCalibrationPoint, SENSOR_CONFIG[0].secondCalibrationPoint),
      soil2_(SENSOR_CONFIG[1].id, SENSOR_CONFIG[1].pin,
        SENSOR_CONFIG[1].firstCalibrationPoint, SENSOR_CONFIG[1].secondCalibrationPoint),
      soil3_(SENSOR_CONFIG[2].id, SENSOR_CONFIG[2].pin,
        SENSOR_CONFIG[2].firstCalibrationPoint, SENSOR_CONFIG[2].secondCalibrationPoint),
      soil4_(SENSOR_CONFIG[3].id, SENSOR_CONFIG[3].pin,
        SENSOR_CONFIG[3].firstCalibrationPoint, SENSOR_CONFIG[3].secondCalibrationPoint),
      soil5_(SENSOR_CONFIG[4].id, SENSOR_CONFIG[4].pin,
        SENSOR_CONFIG[4].firstCalibrationPoint, SENSOR_CONFIG[4].secondCalibrationPoint),
      soil6_(SENSOR_CONFIG[5].id, SENSOR_CONFIG[5].pin,
        SENSOR_CONFIG[5].firstCalibrationPoint, SENSOR_CONFIG[5].secondCalibrationPoint),
      battery_(SENSOR_CONFIG[7].id, SENSOR_CONFIG[7].pin, SENSOR_CONFIG[7].enabled,
     SENSOR_CONFIG[7].firstCalibrationPoint, SENSOR_CONFIG[7].secondCalibrationPoint) {}

bool TelemetryService::begin() {
  if (SENSOR_CONFIG[0].enabled) registry_.add(soil1_);
  if (SENSOR_CONFIG[1].enabled) registry_.add(soil2_);
  if (SENSOR_CONFIG[2].enabled) registry_.add(soil3_);
  if (SENSOR_CONFIG[3].enabled) registry_.add(soil4_);
  if (SENSOR_CONFIG[4].enabled) registry_.add(soil5_);
  if (SENSOR_CONFIG[5].enabled) registry_.add(soil6_);
  if (SENSOR_CONFIG[6].enabled) registry_.add(temperature_);
  registry_.add(battery_);
  registry_.add(battery_);
  initialized_ = registry_.beginAll();
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
