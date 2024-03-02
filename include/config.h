#include "Arduino.h"

#ifndef CONFIG_H
#define CONFIG_H

class Config {
  public:
    Config();

  // INPUT CONFIG
  public:
    uint8_t PIN_DHT = 2;
    uint8_t PIN_MOTION = 3;
    uint8_t BTN_SELECT = 8;
    uint8_t BTN_DOWN = 9;
    uint8_t BTN_UP = 10;

  // OUTPUT CONFIG
  public:
    uint8_t PIN_RGB_R = 5;
    uint8_t PIN_RGB_G = 6;
    uint8_t PIN_RGB_B = 7;
    uint8_t PIN_HVAC_Y1 = A3;
    uint8_t PIN_HVAC_G = A2;
    uint8_t PIN_HVAC_O = A1;
    uint8_t PIN_HVAC_W1 = A0;
    uint8_t PIN_HVAC_W2 = 4;

  // PERIPHERAL CONFIG
  public:
    uint8_t OLED_WIDTH = 128;
    uint8_t OLED_HEIGHT = 64;
    uint8_t SCREEN_RESET = 4;
    uint8_t SCREEN_ADDRESS = 0x3C;

  // CONFIGURABLE SETTINGS
  public:
    unsigned long AVR_RESET = 1000 * 60 * 60 * 24 * 7;
    uint16_t BTN_DEBOUNCE = 200;
    uint16_t UI_AWAKE_TIME = 20000;
    uint16_t MOTION_SENSOR_AWAKE_TIME = 2000;
    uint8_t MOTION_SENSOR_READ_SPEED = 10;
    uint32_t HVAC_SWITCH_DELAY = 180000;
    uint8_t TEMPERATURE_OFFSET = 0;
    uint32_t TARGET_REACHED_CHECK_DELAY = 1000;
    uint32_t TARGET_REACHED_SHUTOFF_DELAY = 1000;
    uint8_t HUMIDITY_OFFSET = 0;
    uint8_t TARGET_TEMP = 72;
    uint8_t TARGET_RANGE = 1;
    uint16_t TRIGGER_DELAY = 3000;

  public:
    void load();
    void save();

  private:
    bool& updateDisplay;
};

#endif
