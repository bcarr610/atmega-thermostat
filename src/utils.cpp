#include "utils.h"

namespace Utils {
  uint8_t celsiusToFahrenheit (uint8_t c) {
    uint8_t calc = round((float)c * 1.8);
    return calc + 32;
  };

  uint8_t floatToUint8(float v) {
    int roundedValue = round(v);
    if (roundedValue < 0) {
      return 0;
    } else if (roundedValue > 255) {
      return 255;
    } else {
      return static_cast<uint8_t>(roundedValue);
    }
  }

  uint8_t averageOf10(uint8_t input[10]) {
    uint32_t sum = 0;
    uint8_t count = 0;
    for (int i = 0; i < 10; i++) {
      uint8_t val = input[i];
      if (val > 0) {
        sum += val;
        count++;
      }
    }

    float avg = sum > 0 && count > 0 ? sum / count : 0;

    if (avg > 0) {
      return floatToUint8(avg);
    } else {
      return 0;
    }
  }
};
