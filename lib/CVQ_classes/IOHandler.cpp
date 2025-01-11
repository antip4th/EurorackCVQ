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
    // Multiplied by factor 2, because of one unused resolution bit
    uint16_t inval = CONVERTERS_FULL_SCALE - (adc.readValue() << 1); 
    return static_cast<uint16_t>(inval * INPUT_GAIN_CORRECTION_FACTOR + INPUT_OFFSET_CORRECTION);
}

void IOHandler::resetADC()
{
    adc.reset();
}

void IOHandler::writeADCOperationConfig()
{
    writeADCRegister(adcOperationConfig, 0x01, 6);
}

void IOHandler::writeADCCalibrationConfig()
{
    writeADCRegister(adcCalibrationConfig, 0x01, 6);
}

void IOHandler::writeADCRegister(uint8_t adcConfig[], uint8_t startAddr, uint8_t len)
{
    // appending command-byte with starting addres to register values
    uint8_t message[len+1] = {static_cast<uint8_t>(COMMAND_BYTE_WRITE_RAW | (startAddr << 2))};
    for (size_t i = 0; i < len; i++)
    {
        message[i+1] = adcConfig[i];
    }
    adc.writeConfig(message, len+1);
}

void IOHandler::startADCConversion()
{
    adc.startConversion();
}

void IOHandler::adcStandby()
{
    adc.standby();
}

void IOHandler::writeCalibration(uint16_t offsetCal, uint16_t gainCal)
{
    uint8_t message[] = {static_cast<uint8_t>((offsetCal & 0xFF00)>>8),
                         static_cast<uint8_t>((offsetCal & 0x00FF)),
                         0x00,
                         static_cast<uint8_t>((gainCal & 0xFF00)>>8),
                         static_cast<uint8_t>((gainCal & 0x00FF)),
                        };
    writeADCRegister(message, 0x09, 5);
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
    uint16_t calibVal = static_cast<uint16_t>(outputVal * OUTPUT_GAIN_CORRECTION_FACTOR + OUTPUT_OFFSET_CORRECTION);
    uint16_t outval = (CONVERTERS_FULL_SCALE - calibVal);
    dac.writeChannel(DAC_CHANNEL_A, outval);
}