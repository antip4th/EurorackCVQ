#include <Arduino.h>
#include <SPI.h>
#include "../../include/defines.h"

#ifndef MCP346xR_H
#define MCP346xR_H

class MCP346xR{
    public:
        MCP346xR(uint8_t SPICSPin = SS, SPISettings *spiSettings = nullptr);
        void reset();
        void writeConfig(uint8_t config[], uint8_t len);
        void writeOffsetCorrection(uint16_t offsetCorrectionVal);
        void startConversion();
        void standby();
        uint8_t getStatus();
        uint16_t readValue();
        bool getDataReady();
    private:
        SPISettings *spiSettings;
        uint8_t SPICSPin;
};

#endif