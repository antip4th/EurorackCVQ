#include <Arduino.h>
#include <SPI.h>
#include <cmath>
#include "defines.h"
#include "DAC7565.h"

#define DAC_FULL_SCALE 65536

double a = 0;
double t = 0; 
uint16_t dacval = 32767; 

DAC dac(-1,-1,-1,-1,-1);

void setup(){
Serial1.begin(115200);
dac.init();
dac.setReference(DAC_REFERENCE_ALWAYS_POWERED_DOWN);
}

void loop(){
    a = 0.5*sin(t) + 0.5;
    t += 0.01;
    dacval = a * (DAC_FULL_SCALE - 1);

    dac.writeChannel(DAC_CHANNEL_A, dacval);
    // dacval++;
    // if(dacval == DAC_FULL_SCALE-1)
    //     Serial1.println(dacval,DEC);
}