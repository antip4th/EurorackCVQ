#include <Arduino.h>
#include <SPI.h>
#include "MCP346xR.h"
#include "defines.h"

void singleConvISR();

// Global variable to hold ACD-Value for ISR-Readout
volatile uint16_t adcval = 0;

// Message for Status-Byte
uint8_t val = 0b01000000;

// define different config-Bytes
uint8_t commandByte = 0b01000110; // Begin incremental write at ADDR 0001
// uint8_t config0 = 0b10000010; // standby mode, no CS, external clock, internal VREF
uint8_t config0 = 0b00000010; // standby mode, no CS, external clock, externel VREF
// uint8_t config0 = 0b10110010; // standby or shutdown mode??, internal clock
// uint8_t config1 = 0b00001100; // default values for now, OSR = 256
uint8_t config1 = 0b00010000; // default values for now, OSR = 512
// uint8_t config1 = 0b00000100; // default values for now, OSR = 64
// uint8_t config1 = 0b00001000; // default values for now, OSR = 128
uint8_t config2 = 0b10001001; // default, settings for BOOST and GAIN
//uint8_t config3 = 0b00000000; // default, setting for data_format and conv_mode
uint8_t config3 = 0b10000000; // default, setting for data_format and standby-mode after singe conv, conv mode = single
// uint8_t config3 = 0b11000000; // default, setting for data_format and standby-mode after singe conv, conv mode = continous
//uint8_t irq = 0b01110011; // default values
uint8_t irq = 0b01110010; // disable conversion start interrupt
uint8_t mux = 0b00001100; // set MUX_VIN+ to CH0, MUX_VIN- to REFIN- (=GND)
//uint8_t mux = 0b00001011; // set MUX_VIN- to REFIN+ (=GND)
uint8_t scan = 0b00001100; // default
uint8_t timer = 0b00001100;

uint8_t configSingleConv[] = {commandByte, config0, config1, config2, config3, irq, mux};

uint16_t value = 0;

void setup(){
  // Light internal LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  
  // Setup GPIO22 for IRQ from ADC
  pinMode(IRQ_INPUT_PIN, INPUT_PULLUP);

  // Setup interrupt pin for ISR
  pinMode(IRQ_INPUT_PIN, INPUT);

  //Output Clock-Signal derived from internal clock to GPIO21
  clock_gpio_init(PICO_CLOCK_OUTPUT_PIN, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 6);

  Serial1.begin(115200); 
  
  // Set up CS-Pin for SPI0
  pinMode(SPI0CS_PIN, OUTPUT);
  digitalWrite(SPI0CS_PIN, HIGH);

  // Set up SPI device on SPI0 w/ default pinout and software-managed CS Pin
  SPI.begin(false);
  
  // Reset MCP3464R
  MCP346xR::reset();

  // // Output internal Clock to pin 18
  // // command byte: Start incremental write at register address 0001 (0x1)
  // uint8_t clkComm1 = 0b01000110;
  // // register contents for 0x01 CONFIG0
  // uint8_t clkComm2 = 0b11110000;
  //
  // uint8_t clk_t_Buf[2] = {clkComm1, clkComm2};
  // uint8_t clk_r_Buf[2] = {};
  //
  // SPI.beginTransaction(MCP346xR::SPIconfig);
  // digitalWrite(SPI0CS_PIN, LOW);
  // SPI.transfer(clk_t_Buf,clk_r_Buf,2);
  // digitalWrite(SPI0CS_PIN, HIGH);
  // SPI.endTransaction();
  
  // // unlock registers for write access (not necessary)
  //
  // uint8_t t_buf[2] = {};
  // t_buf[0] = 0b01110110; //slaveaddr + lockregister + wr
  // t_buf[1] = 0b10100101;
  //
  // SPI.beginTransaction(MCP346xR::SPIconfig);
  // SPI.transfer(t_buf,2);
  // SPI.endTransaction();
  //
  // Program ADC for single conversion operation
  
  // Continously write config to ADC
  SPI.beginTransaction(MCP346xR::SPIconfig);
  digitalWrite(SPI0CS_PIN, LOW);
  SPI.transfer(configSingleConv,7);
  digitalWrite(SPI0CS_PIN, HIGH);
  SPI.endTransaction();

  // // Check for correct configuration (Print to serial)
  // uint8_t configRecBuf[27] = {};
  // configRecBuf[0] = {0b01000011};
  //
  // SPI.beginTransaction(MCP346xR::SPIconfig);
  // digitalWrite(SPI0CS_PIN, LOW);
  // SPI.transfer(configRecBuf,27);
  // digitalWrite(SPI0CS_PIN, HIGH);
  // SPI.endTransaction();
  //
  // uint8_t stringbuffer[8] = {};
  //
  // for (size_t i = 0; i < 27; i++)
  // {
  //  Serial1.println(configRecBuf[i],BIN);
  // }

  attachInterrupt(digitalPinToInterrupt(IRQ_INPUT_PIN), singleConvISR, FALLING);
  MCP346xR::startConversion();

};

// Loop for single-conversions
void loop(){
  //MCP346xR::startConversion();
  //while(digitalRead(IRQ_INPUT_PIN)){};
  //value = MCP346xR::readValue();
  Serial1.print(">ADCValue:");
  Serial1.println(adcval,DEC);
};

// Loop for continous conversions using single conversions
// void loop(){

// }

void singleConvISR(){
  adcval = MCP346xR::readValue();

  // write Val to DAC()
  // Start next conversion, only for Test
  // Weird behaviour: ISR is executed twice, even after clearing the interrupts in the ISR
  
  MCP346xR::startConversion();
  irq_clear(21);
}