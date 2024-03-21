#include "../../include/defines.h"
#include <SPI.h>
#include "DAC7565.h"
#include "MCP346xR.h"

#ifndef IOHANDLER_H
#define IOHANDLER_H

class IOHandler
{
    private:
        const uint8_t COMMAND_BYTE_WRITE_RAW    = 0b01000010; // empty command byte to start writing to registers. need to & with intended write 4-bit address << 2
        const uint8_t CONFIG0                   = 0b00000010; // standby mode, no CS, external clock, externel VREF
        const uint8_t CONFIG1_CONT              = 0b00001000; // default values, OSR = 128
        const uint8_t CONFIG1_CALIB             = 0b00001100; // default values, OSR = 256
        const uint8_t CONFIG2                   = 0b10001001; // default, settings for BOOST and GAIN
        const uint8_t CONFIG3_CONT              = 0b11000000; // default, setting for data_format and standby-mode after singe conv, conv mode = continous
        const uint8_t CONFIG3_CALIB             = 0b10000000; // default, setting for data_format and standby-mode after singe conv, conv mode = single
        const uint8_t IRQ                       = 0b01110010; // disable conversion start interrupt
        const uint8_t MUX                       = 0b00001100; // set MUX_VIN+ to CH0, MUX_VIN- to REFIN- (=GND)

        uint8_t adcCalibrationConfig[7] = {static_cast<uint8_t>(COMMAND_BYTE_WRITE_RAW & (0x01 << 2)), CONFIG0, CONFIG1_CALIB, CONFIG2, CONFIG3_CALIB, IRQ, MUX};
        uint8_t adcOperationConfig[7]   = {static_cast<uint8_t>(COMMAND_BYTE_WRITE_RAW & (0x01 << 2)), CONFIG0, CONFIG1_CONT, CONFIG2, CONFIG3_CONT, IRQ, MUX};

        DAC dac;
        MCP346xR adc;
        SPISettings spiSettings;
        uint8_t spiCSPin;
        
        void writeADCConfig(uint8_t adcConfig[], uint8_t startAddr, uint8_t len);
    
    public:
        // Constructor
        IOHandler(uint32_t spiSpeed, BitOrder spiBitOrder, SPIMode spiMode, uint8_t spiCSPin);
        // general
        void initSPI();
        bool calibrateSystem();

        // ADC
        void resetADC();
        void writeADCOperationConfig();
        void writeADCCalibrationConfig();
        void startADCConversion();
        uint16_t readInputVal();
        
        // DAC
        void initDAC();
        void setDACRefExternal();
        void writeOutputVal(uint16_t outputVal);
};

#endif