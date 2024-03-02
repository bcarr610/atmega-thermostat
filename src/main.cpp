#include <Arduino.h>
#include <avr/wdt.h>
#include "th_sensor.h"
#include "motion_sensor.h"
#include "config.h"
#include "hvac.h"
#include "display.h"

// TODO Add functionality to adjust config;

// State
bool shouldUpdatePreferTmp = true;
bool motionDetected = false;
unsigned long displaySleepAt = 0;
bool isAwake = false;
bool hvacCanExecuteQueue = true;
uint8_t targetTemp = 71;
uint8_t temperature = 0;
uint8_t humidity = 0;
TMP_PREFER preferTmp = PREFER_NEUTRAL;

// Times
unsigned long targetTempReachedAt = 0;
unsigned long triggerChangeAt = 0;
unsigned long lastHvacStateAt = 0;

// Menu State
bool isInMenu = false;

// Button state
bool selectIsPressed = false;
bool downIsPressed = false;
bool upIsPressed = true;
unsigned long nextBtnPressAvailableAt = 0;

// Instances
Config config;
HVAC hvac(config, hvacCanExecuteQueue, shouldUpdatePreferTmp, lastHvacStateAt);
THSensor tmpAndHumiditySensor(config, shouldUpdatePreferTmp);
MotionSensor motionSensor(config, displaySleepAt);
Display display(isAwake, displaySleepAt, config, targetTemp, temperature, humidity, preferTmp, hvac);

// Button Handlers
void onUserTempChange(unsigned long time) {
  if (targetTemp == temperature && hvac.nextState != HVAC_IDLE) {
    hvac.queue(time, HVAC_IDLE);
  } else {
    triggerChangeAt = time + config.TRIGGER_DELAY;
    hvacCanExecuteQueue = false;
  }
}

void onUserInput(unsigned long time) {
  nextBtnPressAvailableAt = time + config.BTN_DEBOUNCE;
  displaySleepAt = time + config.UI_AWAKE_TIME;
}

void onSelectPress(unsigned long time) {
  Serial.println("Select Pressed");
  onUserInput(time);
}

void onDownPress(unsigned long time) {
  if (targetTemp > 50) {
    targetTemp -= 1;
    onUserInput(time);
    onUserTempChange(time);
  }
}

void onUpPress(unsigned long time) {
  if (targetTemp < 80) {
    targetTemp += 1;
    onUserInput(time);
    onUserTempChange(time);
  }
}

// Functions
void resetMicrocontroller() {
  wdt_enable(WDTO_15MS);
  while(1);
}

void setup() {
  // Set initial state
  isAwake = true;
  displaySleepAt = millis() + config.UI_AWAKE_TIME;
  targetTemp = config.TARGET_TEMP;
  
  // Set pin modes
  pinMode(config.PIN_DHT, INPUT);
  pinMode(config.PIN_MOTION, INPUT);
  pinMode(config.BTN_SELECT, INPUT);
  pinMode(config.BTN_DOWN, INPUT);
  pinMode(config.BTN_UP, INPUT);
  pinMode(config.PIN_RGB_R, OUTPUT);
  pinMode(config.PIN_RGB_G, OUTPUT);
  pinMode(config.PIN_RGB_B, OUTPUT);
  pinMode(config.PIN_HVAC_Y1, OUTPUT);
  pinMode(config.PIN_HVAC_G, OUTPUT);
  pinMode(config.PIN_HVAC_O, OUTPUT);
  pinMode(config.PIN_HVAC_W1, OUTPUT);
  pinMode(config.PIN_HVAC_W2, OUTPUT);

  // Set Outputs Low
  digitalWrite(config.PIN_RGB_R, LOW);
  digitalWrite(config.PIN_RGB_G, LOW);
  digitalWrite(config.PIN_RGB_B, LOW);
  digitalWrite(config.PIN_HVAC_Y1, LOW);
  digitalWrite(config.PIN_HVAC_G, LOW);
  digitalWrite(config.PIN_HVAC_O, LOW);
  digitalWrite(config.PIN_HVAC_W1, LOW);
  digitalWrite(config.PIN_HVAC_W2, LOW);
  
  Serial.begin(9600);
  hvac.init();
  tmpAndHumiditySensor.init();
  display.init();

  // Delay to wait for sensors to stabilize 
  delay(1000);

  Serial.println("Ready");
}

void loop() {
  unsigned long now = millis();
  if (now > config.AVR_RESET) {
    resetMicrocontroller();
  }

  motionSensor.tick(now, motionDetected);
  tmpAndHumiditySensor.tick(now, preferTmp, temperature, humidity);
  hvac.tick(now);
  display.tick(now);

  // User Input
  if (now > nextBtnPressAvailableAt) {
    int btnSelectVal = digitalRead(config.BTN_SELECT);
    int btnDownVal = digitalRead(config.BTN_DOWN);
    int btnUpVal = digitalRead(config.BTN_UP);

    if (btnSelectVal == 0 && selectIsPressed == false) {
      selectIsPressed = true;
      onSelectPress(now);
    } else if (btnSelectVal == 1 && selectIsPressed == true) {
      selectIsPressed = false;
    }

    if (btnDownVal == 0 && downIsPressed == false) {
      downIsPressed = true;
      onDownPress(now);
    } else if (btnDownVal == 1 && downIsPressed == true) {
      downIsPressed = false;
    }

    if (btnUpVal == 0 && upIsPressed == false) {
      upIsPressed = true;
      onUpPress(now);
    } else if (btnUpVal == 1 && upIsPressed == true) {
      upIsPressed = false;
    }
  }

  // Manage timeouts and delays
  if (triggerChangeAt > 0 && now >= triggerChangeAt) {
    hvacCanExecuteQueue = true;
    triggerChangeAt = 0;
  }

  if (hvac.isRunning && temperature == targetTemp && targetTempReachedAt == 0) {

  }

  // HVAC Control Logic
  if (hvacCanExecuteQueue && temperature > 0) {
    uint8_t tmpRangeTop;
    uint8_t tmpRangeBottom;

    if (preferTmp == PREFER_NEUTRAL) {
      tmpRangeTop = targetTemp + config.TARGET_RANGE;
      tmpRangeBottom = targetTemp - config.TARGET_RANGE;
    } else if (preferTmp == PREFER_HEAT) {
      tmpRangeTop = targetTemp + config.TARGET_RANGE + 1;
      tmpRangeBottom = targetTemp;
    } else if (preferTmp == PREFER_COOL) {
      tmpRangeTop = targetTemp;
      tmpRangeBottom = targetTemp - config.TARGET_RANGE - 1;
    }

    if (temperature < tmpRangeBottom && hvac.nextState != HVAC_HEATING) {
      Serial.print("Temp is "); Serial.print(temperature); Serial.println(", starting heat.");
      hvac.queue(now, HVAC_HEATING);
    } else if (temperature > tmpRangeTop && hvac.nextState != HVAC_COOLING) {
      Serial.print("Temp is "); Serial.print(temperature); Serial.println(", starting cool.");
      hvac.queue(now, HVAC_COOLING);
    }

    if (
      hvac.currentState != HVAC_IDLE
      && temperature == targetTemp
      && targetTempReachedAt == 0
      && now > lastHvacStateAt + config.TARGET_REACHED_CHECK_DELAY
    ) {
      Serial.println("Target Reached");
      targetTempReachedAt = now;
    }

    if (
      targetTempReachedAt > 0
      && now >= targetTempReachedAt + config.TARGET_REACHED_SHUTOFF_DELAY
    ) {
      Serial.println("Target Reached, Switching back to idle");
      hvac.queue(now, HVAC_IDLE);
      targetTempReachedAt = 0;
    }
    
  }

  // Manage UI Awake State
  if (displaySleepAt > 0 && !isAwake) {
    isAwake = true;
  }

  if (isAwake && displaySleepAt != 0 && now > displaySleepAt) {
    isAwake = false;
    displaySleepAt = 0;
  }
}
