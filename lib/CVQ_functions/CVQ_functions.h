#include <Arduino.h>
#include "DAC7565.h"
#include "MCP346xR.h"
#include "../../include/defines.h"

void blinkInternalLED(uint8_t blinks, bool ledState);

uint16_t external2internal(uint16_t externVal);

uint16_t internal2external(uint16_t internVal);

int16_t calibrateOffset(uint8_t calibrationConfig[], uint8_t configLen, MCP346xR *adc,DAC *dac);

void calibrateGain();