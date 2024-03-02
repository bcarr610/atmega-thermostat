#include "Arduino.h"
#ifndef UTILS_H
#define UTILS_H

namespace Utils {
  extern "C" {
    uint8_t celsiusToFahrenheit(uint8_t c);
    uint8_t floatToUint8(float v);
    uint8_t averageOf10(uint8_t input[10]);
  }
}

#endif
