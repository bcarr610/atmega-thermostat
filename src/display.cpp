#include "display.h"
// #include "SSD1306Ascii.h"
// #include "SSD1306AsciiAvrI2c.h"

Display::Display(
  bool& a,
  unsigned long& s,
  Config& c,
  uint8_t& tt,
  uint8_t& t,
  uint8_t& h,
  TMP_PREFER& p,
  HVAC& hv
):
  isAwake(a),
  sleepAt(s),
  config(c),
  targetTemp(tt),
  temperature(t),
  humidity(h),
  preferTmp(p),
  hvac(hv)
{
}

void Display::init() {
  oled.begin(&Adafruit128x64, 0x3C, -1);
  oled.setFont(System5x7);
  oled.clear();
}

void Display::tick(unsigned long t) {
  if (isAwake == true && isOn == false) {
    show();
  } else if (isAwake == false && isOn == true) {
    hide();
  }
  
  if (isOn == true) {
    uint8_t currentStateMin = hvac.nextStateAt > 0 ? timeDiffMin(hvac.nextStateAt, t) : 0;
    uint8_t currentStateSec = hvac.nextStateAt > 0 ? timeDiffSec(hvac.nextStateAt, t) : 0;
    
    if (lastTemp != temperature) {
      lastTemp = temperature;
      updateTemp(temperature);
    }

    if (lastPreferTmp != preferTmp) {
      lastPreferTmp = preferTmp;
      updatePreferTmp(preferTmp);
    }

    if (lastTarget != targetTemp) {
      lastTarget = targetTemp;
      updateTarget(targetTemp);
    }

    if (lastHumidity != humidity) {
      lastHumidity = humidity;
      updateHumidity(humidity);
    }

    if (lastStateMin != currentStateMin) {
      lastStateMin = currentStateMin;
      updateStateTime(currentStateMin, lastStateSec);
    }

    if (lastStateSec != currentStateSec) {
      lastStateSec = currentStateSec;
      updateStateTime(lastStateMin, currentStateSec);
    }

    if (lastCurrentState != hvac.currentState || lastNextState != hvac.nextState) {
      if (lastCurrentState != hvac.currentState) {
        lastCurrentState = hvac.currentState;
      }

      if (lastNextState != hvac.nextState) {
        lastNextState = hvac.nextState;
      }

      bool isUpcoming = hvac.currentState != hvac.nextState;
      updateState(isUpcoming ? hvac.nextState : hvac.currentState, isUpcoming);
      updateRgb(hvac.currentState);
    }
  }

};

void Display::show() {
  isOn = true;
  writeStatic();
  updateTemp(lastTemp);
  updateTarget(lastTarget);
  updatePreferTmp(lastPreferTmp);
  updateHumidity(lastHumidity);
  if (hvac.currentState != hvac.nextState) {
    updateState(lastNextState, true);
  } else {
    updateState(lastCurrentState, false);
  }
  updateStateTime(lastStateMin, lastStateSec);
  updateRgb(hvac.currentState);
};

void Display::hide() {
  isOn = false;
  oled.clear();
  setRgb(RGB_OFF);
};

void Display::writeStatic() {
  oled.clear();
  oled.setCursor(0, 0);
  oled.print("Current:");
  oled.setCursor(0, 1);
  oled.print("Target:");
  oled.setCursor(0, 2);
  oled.print("---------------------");
  oled.setCursor(0, 3);
  oled.print("Humidity:");
  oled.setCursor(115, 3);
  oled.print("%");
  oled.setCursor(0, 4);
  oled.print("Prefer:");
  oled.setCursor(0, 6);
  oled.print("---------------------");
  oled.setCursor(0, 7);
  oled.print(">");
};

uint8_t Display::timeDiffMin(unsigned long t1, unsigned long t2) {
  unsigned long timeDiff = abs(t1 - t2);
  unsigned long totalSeconds = timeDiff / 1000;
  return totalSeconds / 60;
};
uint8_t Display::timeDiffSec(unsigned long t1, unsigned long t2) {
  unsigned long timeDiff = abs(t1 - t2);
  unsigned long totalSeconds = timeDiff / 1000;
  return totalSeconds % 60;
};

void Display::updateTemp(uint8_t t) {
  oled.setCursor(110, 0);
  if (temperature > 0) oled.print(String(temperature));
  else oled.print("..");
};

void Display::updatePreferTmp(TMP_PREFER d) {
  oled.setCursor(100, 4);
  
  if (preferTmp == PREFER_HEAT) {
    oled.print("heat");
  } else if (preferTmp == PREFER_COOL) {
    oled.print("cool");
  } else {
    oled.print("   -");
  }
  
};

void Display::updateHumidity(uint8_t h) {
  oled.setCursor(100, 3);
  if (humidity > 0) oled.print(String(humidity));
  else oled.print("..");
};

void Display::updateTarget(uint8_t t) {
  oled.setCursor(110, 1);
  oled.print(String(targetTemp));
};

void Display::updateState(HVAC_STATE s, bool isUpcoming) {
  oled.setCursor(10, 7);
  switch(s) {
    case HVAC_BLOWING:
      oled.print("Blow");
      break;
    case HVAC_COOLING:
      oled.print("Cool");
      break;
    case HVAC_HEATING:
      oled.print("Heat");
      break;
    case HVAC_HEATING_ALT1:
      oled.print("Alt1");
      break;
    case HVAC_HEATING_ALT2:
      oled.print("Alt2");
      break;
    case HVAC_IDLE:
    default:
      oled.print("Idle");
      break;
  }
};

void Display::updateStateTime(uint8_t m, uint8_t s) {
  oled.setCursor(35, 7);
  if (m > 0 || s > 0) {
    oled.print("in");
  } else if (m <= 0 && s <= 0) {
    oled.print("  ");
  }
  
  oled.setCursor(50, 7);
  if (m > 0) {
    String sString = s < 10 ? "0" + String(s) : String(s);
    String output = String(m) + ":" + sString;
    oled.print(output);
  } else if (m <= 0 && s > 0) {
    String output = String(s) + "s    ";
    oled.print(output);
  } else if (m <= 0 && s <= 0) {
    oled.print("     ");
  }
};

void Display::updateRgb(HVAC_STATE state) {
  if (state == HVAC_IDLE) setRgb(RGB_WHITE);
  if (state == HVAC_BLOWING) setRgb(RGB_GREEN);
  if (state == HVAC_COOLING) setRgb(RGB_BLUE);
  if (state == HVAC_HEATING) setRgb(RGB_RED);
  if (state == HVAC_HEATING_ALT1) setRgb(RGB_PURPLE);
  if (state == HVAC_HEATING_ALT2) setRgb(RGB_YELLOW);
}

void Display::setRgb(RGB_COLOR color) {
  byte values[3] = { LOW, LOW, LOW };

  switch(color) {
    case RGB_RED:
      values[0] = HIGH;
      break;
    case RGB_GREEN:
      values[1] = HIGH;
      break;
    case RGB_BLUE:
      values[2] = HIGH;
      break;
    case RGB_YELLOW:
      values[0] = HIGH;
      values[1] = HIGH;
      break;
    case RGB_PURPLE:
      values[0] = HIGH;
      values[2] = HIGH;
      break;
    case RGB_CYAN:
      values[1] = HIGH;
      values[2] = HIGH;
      break;
    case RGB_WHITE:
      values[0] = HIGH;
      values[1] = HIGH;
      values[2] = HIGH;
      break;
    case RGB_OFF:
    default:
      break;
  }

  digitalWrite(config.PIN_RGB_R, values[0]);
  digitalWrite(config.PIN_RGB_G, values[1]);
  digitalWrite(config.PIN_RGB_B, values[2]);
  
}
