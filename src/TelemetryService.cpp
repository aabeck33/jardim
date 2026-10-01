#include <services/TelemetryService.h>
#include <main.h>
#include <utils.h>

TelemetryService::TelemetryService()
    : soil1_("solo_1", pinosEntrada[0]),
      soil2_("solo_2", pinosEntrada[1]),
      soil3_("solo_3", pinosEntrada[2]),
      soil4_("solo_4", pinosEntrada[3]),
      soil5_("solo_5", pinosEntrada[4]),
      soil6_("solo_6", pinosEntrada[5]),
      battery_("bateria", VBAT_READ, USE_BATTERY) {}

bool TelemetryService::begin() {
  registry_.add(soil1_);
  registry_.add(soil2_);
  registry_.add(soil3_);
  registry_.add(soil4_);
  registry_.add(soil5_);
  registry_.add(soil6_);
  registry_.add(temperature_);
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

  const SensorReading temperature = temperature_.reading();
  data["temperatura"] = temperature.calibratedValue;
  data["temperatura_unidade"] = temperature.unit;
  data["temperatura_estado"] = temperature.state;

  const SensorReading battery = battery_.reading();
  if (battery.valid) {
    data["bateria"] = battery.calibratedValue;
  } else {
    data["bateria"] = nullptr;
  }
  data["bateria_unidade"] = battery.unit;
  data["bateria_estado"] = battery.state;

  JsonArray sensors = data.createNestedArray("sensores");
  registry_.appendTo(sensors);

  JsonArray soil = data.createNestedArray("umidade");
  registry_.appendTo(soil, "umidade_solo");

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
