#include <Arduino.h>
#include <SPI.h>
#include "defines.h"
#include "DAC7565.h"
#include "MCP346xR.h"
#include "./CVQ_functions.h"
#include "IOHandler.h"

// ISR to read from ADC

void performConversionISR();

// Global variable to hold ACD-Value for ISR-Readout
volatile uint16_t inputVal = 0;

// Create IOHandler
IOHandler ioHandler(SPI_SPEED_20MHZ, MSBFIRST, SPI_MODE3, SPI0CS_PIN);

void setup(){
    
    // Pin setup //

    // Set internal LED Pin internal LED
    pinMode(LED_BUILTIN, OUTPUT);

    // Setup GPIO22 for IRQ from ADC
    pinMode(IRQ_INPUT_PIN, INPUT);

    //Output Clock-Signal for ADC derived from internal clock to GPIO21
    clock_gpio_init(PICO_CLOCK_OUTPUT_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 10);

    // Set up Calibartion Push-Button Input Pin
    pinMode(PUSH_BUTTON_PIN, INPUT);

    // Set up conversion mode switch Pin (ToDo)

    // Initialize IO Handler

    ioHandler.initSPI();
    ioHandler.resetADC();
    ioHandler.writeADCOperationConfig();

    ioHandler.setDACRefExternal();

    // perform calibration??

    // begin normal operation

    attachInterrupt(digitalPinToInterrupt(IRQ_INPUT_PIN), performConversionISR, FALLING);
    ioHandler.startADCConversion();

    digitalWrite(LED_BUILTIN, HIGH);
};

void loop(){
// Test DAC function
//   uint16_t x = 0;
//   while(1){
//     ioHandler.writeOutputVal(x);
//     x += 50;
//   }
};


void performConversionISR()
{
    inputVal = ioHandler.readInputVal();
    ioHandler.writeOutputVal(inputVal);
};