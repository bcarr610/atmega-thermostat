#include "Arduino.h"
#include "config.h"

#ifndef HVAC_H
#define HVAC_H

enum HVAC_STATE {
  HVAC_IDLE,
  HVAC_BLOWING,
  HVAC_COOLING,
  HVAC_HEATING,
  HVAC_HEATING_ALT1,
  HVAC_HEATING_ALT2,
};

class HVAC {
  public:
    HVAC(
      Config& config,
      bool& hvacCanExecuteQueue,
      bool& supt,
      unsigned long& lhsa
    );

  private:
    Config& config;
    bool& shouldUpdatePreferTmp;
    bool& canExecuteQueue;
    int coolingWire;
    int fanWire;
    int heatPumpWire;
    int altHeat1Wire;
    int altHeat2Wire;
    unsigned long switchDelay;
    unsigned long lastIdleStateAt = 0;
    unsigned long& lastHvacStateAt;

  public:
    bool isRunning = false;
    unsigned long nextStateAt = 0;
    HVAC_STATE currentState = HVAC_IDLE;
    HVAC_STATE nextState = HVAC_IDLE;

  public:
    void init();
    void queue(unsigned long time, HVAC_STATE state);
    void tick(unsigned long time);

  private:
    void executeNextState(unsigned long time);
    void killRelays();
};

#endif
