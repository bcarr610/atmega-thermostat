#ifndef DISPLAY_H;
#define DISPLAY_H;
#include "Arduino.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "hvac.h"
#include "config.h"
#include "th_sensor.h"

enum RGB_COLOR {
  RGB_OFF,
  RGB_RED,
  RGB_BLUE,
  RGB_GREEN,
  RGB_YELLOW,
  RGB_PURPLE,
  RGB_WHITE,
  RGB_CYAN,
};

class Display {
  public:
    Display(
      bool& a,
      unsigned long& s,
      Config& c,
      uint8_t& tt,
      uint8_t& t,
      uint8_t& h,
      TMP_PREFER& tp,
      HVAC& hv
    );

  private:
    unsigned long& sleepAt;
    bool& isAwake;
    bool isOn = false;
    uint8_t oledCharWidth = 5;
    uint8_t oledCols = 24;
    uint8_t oledRows = 7;

  private:
    Config& config;
    SSD1306AsciiAvrI2c oled;
    uint8_t& targetTemp;
    uint8_t& temperature;
    uint8_t& humidity;
    TMP_PREFER& preferTmp;
    HVAC& hvac;

  private:
    uint8_t lastTemp = 0;
    TMP_PREFER lastPreferTmp = PREFER_NEUTRAL;
    uint8_t lastTarget = 0;
    uint8_t lastHumidity = 0;
    HVAC_STATE lastCurrentState = HVAC_IDLE;
    HVAC_STATE lastNextState = HVAC_IDLE;
    uint8_t lastStateMin = 0;
    uint8_t lastStateSec = 0;
  
  public:
    void init();
    void tick(unsigned long t);
    void show();
    void hide();

  private:
    void writeStatic();
    void updateTemp(uint8_t t);
    void updatePreferTmp(TMP_PREFER p);
    void updateTarget(uint8_t t);
    void updateHumidity(uint8_t h);
    void updateState(HVAC_STATE s, bool isUpcoming);
    void updateStateTime(uint8_t m, uint8_t s);
    void updateRgb(HVAC_STATE s);
    
  private:
    void setRgb(RGB_COLOR color);
    uint8_t timeDiffMin(unsigned long t1, unsigned long t2);
    uint8_t timeDiffSec(unsigned long t1, unsigned long t2);
};

#endif
