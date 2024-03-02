#ifndef TH_SENSOR_H
#define TH_SENSOR_H
#include "Arduino.h"
#include "config.h"
#include "DHT.h"
#include "DHT_U.h"
#include "Adafruit_Sensor.h"
#include "utils.h"

enum TMP_PREFER {
  PREFER_HEAT,
  PREFER_NEUTRAL,
  PREFER_COOL,
};

class THSensor {
  public:
    THSensor(Config& config, bool& supt);

  public:
    float tmpMin;
    float tmpMax;
    float tmpRes;
    float hMin;
    float hMax;
    float hRes;

  private:
    DHT_Unified dht;
    sensor_t sensor;
    unsigned long delayMS;
    unsigned long nextReadAvailableAt;
    Config& config;
    bool& shouldUpdatePreferTmp;
    uint8_t lastRecordedTemps[10];
    uint8_t lastRecordedHumidity[10];
    uint8_t currentRecordIndex = 0;

  private:
    uint8_t getTemperature(sensors_event_t& event);
    uint8_t getHumidity(sensors_event_t& event);

  public:
    void init();
    void read(TMP_PREFER& preferTmp, uint8_t& temperature, uint8_t& humidity);
    void tick(unsigned long time, TMP_PREFER& preferTmp, uint8_t& temperature, uint8_t& humidity);
};

#endif
