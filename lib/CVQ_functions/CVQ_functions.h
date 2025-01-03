#include <Arduino.h>
#include "DAC7565.h"
#include "MCP346xR.h"
#include "../../include/defines.h"

void blinkInternalLED(uint8_t blinks, bool ledState, uint16_t msDelay=200);

void waitForPinAction(uint8_t pin, PinStatus polarity, uint16_t msSleep);