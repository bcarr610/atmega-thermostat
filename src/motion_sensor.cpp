#include "motion_sensor.h"

MotionSensor::MotionSensor(Config& c,unsigned long& dst): config(c), displaySleepAt(dst) {
  pin = config.PIN_MOTION;
  timeout = config.MOTION_SENSOR_AWAKE_TIME;
  readDelay = config.MOTION_SENSOR_READ_SPEED;
  isHigh = false;
};

void MotionSensor::tick(unsigned long time, bool& detected) {
  if (time >= readAvailableAt) {

    if (digitalRead(pin) == 1) {
      if (isHigh == false) isHigh = true;
    } else {
      if (isHigh == true) isHigh = false;
    }

    if (isHigh) {
      if (detected == false) {
        onDetect(time);
        detected = true;
      }
      stayOnUntil = time + timeout;
    } else {
      if (detected == true && time >= stayOnUntil) {
        onTimeout(time);
        detected = false;
        stayOnUntil = 0;
      }
    }

    readAvailableAt = time + readDelay;
  }
}

void MotionSensor::onDetect(unsigned long time) {
  displaySleepAt = time + config.UI_AWAKE_TIME;
};

void MotionSensor::onTimeout(unsigned long time) {
};
