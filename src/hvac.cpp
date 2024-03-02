#include "hvac.h"

HVAC::HVAC(
  Config& config,
  bool& hvacCanExecuteQueue,
  bool& supt,
  unsigned long& lhsa
):
  config(config),
  canExecuteQueue(hvacCanExecuteQueue),
  shouldUpdatePreferTmp(supt),
  lastHvacStateAt(lhsa)
{
  coolingWire = config.PIN_HVAC_Y1;
  fanWire = config.PIN_HVAC_G;
  heatPumpWire = config.PIN_HVAC_O;
  altHeat1Wire = config.PIN_HVAC_W1;
  altHeat2Wire = config.PIN_HVAC_W2;
  switchDelay = config.HVAC_SWITCH_DELAY;
}

void HVAC::queue(unsigned long time, HVAC_STATE state) {
  if (state == HVAC_IDLE) {
    nextState = HVAC_IDLE;
    nextStateAt = 0;
    executeNextState(time);
  } else if (state != currentState && state != nextState) {

    // If requesting change, idle first
    if (currentState != HVAC_IDLE) {
      nextState = HVAC_IDLE;
      executeNextState(time);
    }

    nextState = state;

    // Set new start time if not currently waiting
    if (nextStateAt == 0 || time >= nextStateAt) {
      nextStateAt = time + switchDelay;
    }
  }
}

void HVAC::tick(unsigned long time) {
  if (currentState != HVAC_IDLE && isRunning == false) {
    isRunning = true;
    shouldUpdatePreferTmp = false;
  } else if (currentState == HVAC_IDLE && isRunning == true) {
    isRunning = false;
    shouldUpdatePreferTmp = true;
    lastIdleStateAt = time;
  }
  
  if (canExecuteQueue && nextStateAt != 0) {
    if (time > nextStateAt && nextState != currentState) {
      executeNextState(time);
    }
  }
}

void HVAC::init() {
  killRelays();
}

void HVAC::killRelays() {
  digitalWrite(coolingWire, LOW);
  digitalWrite(fanWire, LOW);
  digitalWrite(heatPumpWire, LOW);
  digitalWrite(altHeat1Wire, LOW);
  digitalWrite(altHeat2Wire, LOW);
}

void HVAC::executeNextState(unsigned long time) {
  killRelays();
  delay(20);
  switch (nextState) {
    case HVAC_BLOWING:
      digitalWrite(fanWire, HIGH);
      break;
    case HVAC_COOLING:
      digitalWrite(coolingWire, HIGH);
      break;
    case HVAC_HEATING:
      digitalWrite(heatPumpWire, HIGH);
      break;
    case HVAC_HEATING_ALT1:
      digitalWrite(altHeat1Wire, HIGH);
      break;
    case HVAC_HEATING_ALT2:
      digitalWrite(altHeat2Wire, HIGH);
      break;
    case HVAC_IDLE:
    default:
      killRelays();
      break;
  }

  currentState = nextState;
  nextStateAt = 0;
  lastHvacStateAt = time;
}
