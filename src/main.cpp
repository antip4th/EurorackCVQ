#include <Arduino.h>
#include <SPI.h>
#include "defines.h"
#include "DAC7565.h"
#include "MCP346xR.h"
#include "IOHandler.h"
#include "./CVQ_functions.h"
#include "conversionLUT.h"

#define DEBUG_MODE false
#define NUM_CALIB_CONV 300
#define NUM_CALIB_MEAS 2

// ISR to read from ADC

void performConversionISR();
void calibrationISR();

// Global variable to hold ACD-Value for ISR-Readout
volatile uint16_t inputVal = 0;
volatile uint16_t outputVal = 0;

volatile uint32_t calibAccu = 0;
volatile uint16_t measCounter = 0;

// Create IOHandler
IOHandler ioHandler(SPI_SPEED_20MHZ, MSBFIRST, SPI_MODE3, SPI0CS_PIN);

void setup()
{
    
    // Pin setup //

    // Set internal LED Pin internal LED
    pinMode(LED_BUILTIN, OUTPUT);

    // Setup GPIO22 for IRQ from ADC
    pinMode(IRQ_INPUT_PIN, INPUT);

    //Output Clock-Signal for ADC (13.3 MHz) derived from internal clock to GPIO21
    clock_gpio_init(PICO_CLOCK_OUTPUT_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 10);

    // Set up voltage range switch position (ToDo)

    // Set up Calibartion Push-Button Input Pin
    pinMode(PUSH_BUTTON_PIN, INPUT);

    // Set up conversion mode input Pin (ToDo)

    // Start up serial Port for debugging

    Serial1.begin(115200);

    // Initialize IO Handler //

    ioHandler.initSPI();
    ioHandler.resetADC();
    ioHandler.writeADCOperationConfig();
    ioHandler.setDACRefExternal();

    // perform calibration procedure if push-button pressed

    //table of common corrensponding calibration values:

    // mV      | LSB
    // -----------------
    // 1000    | 6554
    // 3000    | 19661
    // 5000    | 32768
    // 7000    | 45875
    // 9000    | 58982	

    uint16_t calibIntValues[NUM_CALIB_MEAS] = {6554, 58982};
    float calibVoltValues[NUM_CALIB_MEAS]   = {1000.0, 9000.0};
    
    if(!digitalRead(PUSH_BUTTON_PIN))
    {
        Serial1.println("CVQ Calibration: Input");
        
        // debounce switch / wait for release
        do
        {
        sleep_ms(100);
        }
        while(!digitalRead(PUSH_BUTTON_PIN));

        // do input measurements
        uint16_t measAvg[NUM_CALIB_MEAS] = {};
        for(int i = 0; i<NUM_CALIB_MEAS; i++)
        {
            blinkInternalLED(i+1,0);
            Serial1.printf("Set input Voltage to %.1f mV\n", calibVoltValues[i]);
            Serial1.println("Press button to start measurement.");
            waitForPinAction(PUSH_BUTTON_PIN, FALLING, 50);
            Serial1.printf("%d/%d: Measuring...\n", i+1, NUM_CALIB_MEAS);
            
            // Set up new series of measurements
            noInterrupts();
            measCounter = 0;
            calibAccu = 0;
            attachInterrupt(digitalPinToInterrupt(IRQ_INPUT_PIN), calibrationISR, FALLING);
            interrupts();
            ioHandler.startADCConversion();
            // Wait while measuring
            while(measCounter < NUM_CALIB_CONV){}
            ioHandler.adcStandby();
            // Print result
            // TOFDO: Rework division!
            measAvg[i] = static_cast<uint16_t>(calibAccu/NUM_CALIB_CONV);
            Serial1.printf("Average = %d\n", measAvg[i]);
        }

        Serial1.println("Input Measurements done");
        // blink fast to signal end of input measurements
        blinkInternalLED(10,0,50);

        // disable ADC clocking for better measurement
        pinMode(PICO_CLOCK_OUTPUT_PIN, INPUT_PULLDOWN);

        // write output values to 
        for(int i = 0; i<NUM_CALIB_MEAS; i++)
        {
            blinkInternalLED(i,0);
            Serial1.printf("%d/%d: Setting Output to %d\n", i+1, NUM_CALIB_MEAS, calibIntValues[i]);
            ioHandler.writeOutputVal(calibIntValues[i]);
            Serial1.println("Press button for next value.");
            waitForPinAction(PUSH_BUTTON_PIN, FALLING, 50);
        }
        // blink fast to signal end of output measurements
        blinkInternalLED(10,0,50);
        waitForPinAction(PUSH_BUTTON_PIN, FALLING, 50);
        for (size_t i = 0; i < NUM_CALIB_MEAS; i++)
        {
            Serial1.printf("Input Measurement %d: %d\n", i, measAvg[i]);
        }
        Serial1.println("Press Button to continue");       
        waitForPinAction(PUSH_BUTTON_PIN, FALLING, 50);
        Serial1.println("Calibration procedure done. Proceeding with normal operation.");
        clock_gpio_init(PICO_CLOCK_OUTPUT_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 10);
    }

    // begin normal operation
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(IRQ_INPUT_PIN), performConversionISR, FALLING);
    ioHandler.startADCConversion();
    interrupts();

    digitalWrite(LED_BUILTIN, HIGH);
};

void loop()
{
    if(DEBUG_MODE == true)
    {
        Serial1.print("Input: ");
        Serial1.print(inputVal, DEC);
        Serial1.print(" | Output: ");
        Serial1.println(outputVal, DEC);
        sleep_ms(100);
    }
};

void performConversionISR()
{
    inputVal = ioHandler.readInputVal();
    outputVal = conversionLUT[inputVal];
    ioHandler.writeOutputVal(outputVal);
};

void calibrationISR()
{
    if(measCounter<NUM_CALIB_CONV)
    {
        calibAccu += ioHandler.readInputVal();
        measCounter++;
    }
};