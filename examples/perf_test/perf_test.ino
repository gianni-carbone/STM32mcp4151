#include "STM32mcp4151.h"

STM32mcp4151 pot(PB9);

void setup() {
  Serial.begin(115200);
  pot.continuos(true);
}

uint32_t t;

void loop() {
  t=micros();
  for(uint16_t i= 0; i< 1000;i++) {
    pot.wiper(0x80);
  }
  t=micros()-t;
  Serial.printf("Total:%lu uS Single: %lu uS\n", t, t/1000);
  delay(2000);
  
}
