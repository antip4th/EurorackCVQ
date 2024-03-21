#include "CVQ_functions.h"
#include "MCP346xR.h"
#include "DAC7565.h"


void blinkInternalLED(uint8_t blinks, bool ledState)
{
    if(ledState == HIGH)
    {
        for (size_t i = 0; i < blinks; i++)
        {
        digitalWrite(LED_BUILTIN,LOW);
        sleep_ms(200);
        digitalWrite(LED_BUILTIN,HIGH);
        sleep_ms(200);
        }
    }
    if(ledState == LOW)
    {
        for (size_t i = 0; i < blinks; i++)
        {
        digitalWrite(LED_BUILTIN, HIGH);
        sleep_ms(200);
        digitalWrite(LED_BUILTIN, LOW);
        sleep_ms(200);
        }
    }
}

int16_t calibrateOffset(uint8_t calibrationConfig[], uint8_t configLen, MCP346xR *adc, DAC *dac)
{
    int16_t offsetErrorVal = 0;
    uint16_t numSamples = 1000; 
    
    // Wait for "0V at input" button press
    while(digitalRead(PUSH_BUTTON_PIN))
    {
        sleep_ms(100);
    }
       
    // set ADC to single conv mode
    adc->writeConfig(calibrationConfig, configLen);
    adc->startConversion();
    
    // take many measurements
    for (size_t i = 0; i < numSamples; i++)
    {
        while(!adc->getDataReady()){
            sleep_ms(1);
        }
        offsetErrorVal += adc->readValue();
        Serial1.println(offsetErrorVal);
        adc->startConversion();
    }
    // average over these measurements
    offsetErrorVal = static_cast<int16_t>(round(offsetErrorVal/numSamples));
    
    Serial1.print("Input Offset Value: ");
    Serial1.print(offsetErrorVal);
    Serial1.println("LSB");

    // send value to output
    dac->writeChannel(DAC_CHANNEL_A, CONVERTERS_FULL_SCALE - offsetErrorVal<<1);
    // signal 0V measurement finished
    blinkInternalLED(2, 1);

    // Wait for "output at input" button press
    while(digitalRead(PUSH_BUTTON_PIN))
    {
        sleep_ms(100);
    }

    // take many measurements
    for (size_t i = 0; i < numSamples; i++)
    {
        while(!adc->getDataReady()){
            sleep_ms(1);
        }
        offsetErrorVal += adc->readValue();
        adc->startConversion();
    }

    // average over these measurements
    offsetErrorVal /= numSamples;
    
    return -offsetErrorVal;
}

void calibrateGain()
{
}
