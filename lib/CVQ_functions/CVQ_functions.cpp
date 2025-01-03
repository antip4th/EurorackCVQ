#include "CVQ_functions.h"
#include "MCP346xR.h"
#include "DAC7565.h"


void blinkInternalLED(uint8_t blinks, bool ledState, uint16_t msDelay)
{
    if(ledState == HIGH)
    {
        for (size_t i = 0; i < blinks; i++)
        {
        digitalWrite(LED_BUILTIN,LOW);
        sleep_ms(msDelay);
        digitalWrite(LED_BUILTIN,HIGH);
        sleep_ms(msDelay);
        }
    }
    if(ledState == LOW)
    {
        for (size_t i = 0; i < blinks; i++)
        {
        digitalWrite(LED_BUILTIN, HIGH);
        sleep_ms(msDelay);
        digitalWrite(LED_BUILTIN, LOW);
        sleep_ms(msDelay);
        }
    }
}

void waitForPinAction(uint8_t pin, PinStatus polarity, uint16_t msSleep)
{
    if(polarity == FALLING)
    {
        while(digitalRead(pin))
        {
            sleep_ms(msSleep);
        }
        do
        {
            sleep_ms(msSleep);
        } while(!digitalRead(pin));
    } else 
    {   
        if (polarity == RISING)
        {
            while(!digitalRead(pin))
            {
                sleep_ms(msSleep);
            }
            do
            {
                sleep_ms(msSleep);
            } while(digitalRead(pin));
        }
    }
}
