#include "MCP346xR.h"

MCP346xR::MCP346xR(uint8_t SPICSPin, SPISettings *spiSettings){
    this->SPICSPin = SPICSPin;
    this->spiSettings = spiSettings;
}

void MCP346xR::reset(){
    // Resets the MCP3461 using 1110 fast-command
    SPI.beginTransaction(*this->spiSettings);
    digitalWrite(SPICSPin, LOW);
    SPI.transfer(0b01111000);  // 0b01 1110 00 = Full Reset
    digitalWrite(SPICSPin, HIGH);
    SPI.endTransaction();
}

void MCP346xR::writeConfig(uint8_t config[], uint8_t len){
    // Continously writes bytes to SPI bus
    SPI.beginTransaction(*this->spiSettings);
    digitalWrite(SPICSPin, LOW);
    SPI.transfer(config,len);
    digitalWrite(SPICSPin, HIGH);
    SPI.endTransaction(); 
}

void MCP346xR::writeOffsetCorrection(uint16_t offsetCorrectionVal){
    uint8_t commandByte = 0b01100110; // start write at address 0x9 for OFFSET CALIBRATION REGISTER 
    uint8_t offsetCorrectionVal1 = static_cast<uint8_t>((offsetCorrectionVal & 0xFF00)>>8); // upper part
    uint8_t offsetCorrectionVal2 = static_cast<uint8_t>((offsetCorrectionVal & 0x00FF)); // lower part
    uint8_t config[] = {commandByte, offsetCorrectionVal1, offsetCorrectionVal2, 0x0};
    MCP346xR::writeConfig(config, 4);
};

void MCP346xR::startConversion(){
    // Puts the MCP3461 into conversion mode using 1010 fast-command
    SPI.beginTransaction(*this->spiSettings);
    digitalWrite(SPICSPin, LOW);
    SPI.transfer(0b01101000);
    digitalWrite(SPICSPin, HIGH);
    SPI.endTransaction();
};

void MCP346xR::standby(){
    // Puts the MCP3461 into standby mode using 1011 fast-command
    SPI.beginTransaction(*this->spiSettings);
    digitalWrite(SPICSPin, LOW);
    SPI.transfer(0b01101100);
    digitalWrite(SPICSPin, HIGH);
    SPI.endTransaction();
};

uint8_t MCP346xR::getStatus(){
    SPI.beginTransaction(*this->spiSettings);
    digitalWrite(SPICSPin, LOW);
    uint8_t status = SPI.transfer(0b01000000);
    digitalWrite(SPICSPin, HIGH);  
    SPI.endTransaction();
    return status;
};

uint16_t MCP346xR::readValue(){
    uint8_t buf[3] = {0b01000001, 0, 0};
    SPI.beginTransaction(*this->spiSettings);
    digitalWrite(SPICSPin, LOW);
    SPI.transfer(buf,3);
    digitalWrite(SPICSPin, HIGH);
    SPI.endTransaction();
    uint16_t value = static_cast<uint16_t>(buf[1]<<8) | buf[2]; // combine two uint8_t to uint16_t
    return value; 
};

bool MCP346xR::getDataReady(){
    return !(getStatus() & 0b00000100);
};