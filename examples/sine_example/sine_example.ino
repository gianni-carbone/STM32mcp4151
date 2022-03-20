// *************************************************
// example for bluepill SPI1 and MCP 4151
//  IC        MCU
//  CS        B9
//  CLK       A5
//  DAT       A7
//  VDD       +5v
//  VSS       GND
//  P0B       +5V
//  P0A       GND
//  P0W       output signal  
// ************************************************* 


#include "STM32mcp4151.h"

const uint16_t sin_tbl32[32] = {
  128, 153, 177, 199, 219, 234, 246, 254, 
  256, 254, 246, 234, 219, 199, 177, 153, 
  128, 103,  79,  57,  37,  22,  10,   2, 
    0,   2,  10,  22,  37,  57,  79, 103
};

STM32mcp4151 pot(PB9); // default SPI1 will be used. CSpin to PB9. Use: STM32mcp4151 pot(CS_PIN, SPIx) to use different cs pins and SPIs

void runSine32(){
  // for STM32F103@72MHz. output 878Hz with stock spi driver and 17.5KHz with high perf spi driver
  uint8_t i = 0;
  while(1) {
    pot.wiper(sin_tbl32[i]);
    i++;
    if (i==32) i = 0;
  }
}


void setup() {
  pot.continuos(true);
  runSine32();  
}

void loop() {
}
