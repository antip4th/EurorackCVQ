#include "IOHandler.h"

IOHandler::IOHandler(uint32_t _spiSpeed, BitOrder _spiBitOrder, SPIMode _spiMode, uint8_t _spiCSPin)
{
    spiSettings = SPISettings(_spiSpeed, _spiBitOrder, _spiMode);
    spiCSPin = _spiCSPin;
    dac = DAC(spiCSPin,255,255,255,255, &spiSettings);
    adc = MCP346xR(spiCSPin, &spiSettings);
}

void IOHandler::initSPI()
{
    // Set up CS-Pin for SPI0
    pinMode(spiCSPin, OUTPUT);
    digitalWrite(spiCSPin, HIGH);
    // Set up SPI device on SPI0 w/ default pinout and software-managed CS Pin
    SPI.begin(false);
}

bool IOHandler::calibrateSystem()
{
    return false; // To be implemented
}

// ADC


uint16_t IOHandler::readInputVal()
{
    return CONVERTERS_FULL_SCALE - (adc.readValue() << 1); // Multiplied by factor 2, because of one unused resolution bit
}

void IOHandler::resetADC()
{
    adc.reset();
}

void IOHandler::writeADCOperationConfig()
{
    adc.writeConfig(adcOperationConfig, 7);
}

void IOHandler::writeADCCalibrationConfig()
{
    adc.writeConfig(adcCalibrationConfig, 7);
}

void IOHandler::writeADCConfig(uint8_t adcConfig[], uint8_t startAddr, uint8_t len)
{
    adc.writeConfig(adcConfig, len); // To changed to start writing at a specific address
}

void IOHandler::startADCConversion()
{
    adc.startConversion();
}

//DAC

void IOHandler::initDAC()
{
    dac.init();
}

void IOHandler::setDACRefExternal()
{
    dac.setReference(DAC_REFERENCE_ALWAYS_POWERED_DOWN);
}

void IOHandler::writeOutputVal(uint16_t outputVal)
{
    dac.writeChannel(DAC_CHANNEL_A, CONVERTERS_FULL_SCALE - outputVal);
}
