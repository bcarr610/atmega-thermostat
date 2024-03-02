#include "th_sensor.h"

THSensor::THSensor(Config& config, bool& supt):
  dht(config.PIN_DHT, DHT11),
  config(config),
  shouldUpdatePreferTmp(supt)
{};

void THSensor::init() {
  dht.begin();
  dht.temperature().getSensor(&sensor);
  tmpMin = Utils::celsiusToFahrenheit(sensor.min_value);
  tmpMax = Utils::celsiusToFahrenheit(sensor.max_value);
  tmpRes = Utils::celsiusToFahrenheit(sensor.resolution) - 32;

  dht.humidity().getSensor(&sensor);
  hMin = sensor.min_value;
  hMax = sensor.max_value;
  hRes = sensor.resolution;
  delayMS = sensor.min_delay / 1000;
}

uint8_t THSensor::getTemperature(sensors_event_t& event) {
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature"));
  } else {

    uint8_t temp = Utils::floatToUint8(event.temperature) + config.TEMPERATURE_OFFSET;
    return Utils::celsiusToFahrenheit(temp);
  }
}

uint8_t THSensor::getHumidity(sensors_event_t& event) {
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity"));
  } else {
    return Utils::floatToUint8(event.relative_humidity) + config.HUMIDITY_OFFSET;
  }
}

void THSensor::read(TMP_PREFER& preferTmp, uint8_t& temperature, uint8_t& humidity) {
  sensors_event_t event;
  uint8_t newTemp = getTemperature(event);
  uint8_t newHumidity = getHumidity(event);

  uint8_t currentAvgTemp = Utils::averageOf10(lastRecordedTemps);
  uint8_t currentAvgHumidity = Utils::averageOf10(lastRecordedHumidity);

  lastRecordedTemps[currentRecordIndex] = newTemp;
  lastRecordedHumidity[currentRecordIndex] = newHumidity;

  uint8_t newAvgTemp = Utils::averageOf10(lastRecordedTemps);
  uint8_t newAvgHumidity = Utils::averageOf10(lastRecordedHumidity);

  if (currentAvgTemp > 0 && currentAvgHumidity > 0) {
    if (shouldUpdatePreferTmp) {
      if (newAvgTemp > currentAvgTemp && preferTmp != PREFER_COOL) {
        preferTmp = PREFER_COOL;
      } else if (newAvgTemp < currentAvgTemp && preferTmp != PREFER_HEAT) {
        preferTmp = PREFER_HEAT;
      }
    }
    
    temperature = newAvgTemp;
    humidity = newAvgHumidity;
  }

  if (currentRecordIndex >= 9) {
    currentRecordIndex = 0;
  } else {
    currentRecordIndex++;
  }

  nextReadAvailableAt = millis() + (sensor.min_delay / 1000);
}

void THSensor::tick(unsigned long time, TMP_PREFER& preferTmp, uint8_t& temperature, uint8_t& humidity) {
  if (time > nextReadAvailableAt) {
    read(preferTmp, temperature, humidity);
    nextReadAvailableAt = millis() + (sensor.min_delay / 1000);
  }
}
