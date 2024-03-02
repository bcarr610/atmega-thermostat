#include "Arduino.h"
#include "config.h"

#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

class MotionSensor {
  public:
    MotionSensor(Config& c, unsigned long& dst);

  private:
    Config& config;
    unsigned long& displaySleepAt;
    uint8_t pin;
    bool isHigh = false;
    uint16_t readDelay;
    uint16_t timeout;
  
  private:
    unsigned long stayOnUntil;
    unsigned long readAvailableAt;

  public:
    void tick(unsigned long time, bool& detected);
    void onDetect(unsigned long time);
    void onTimeout(unsigned long time);
};

#endif
