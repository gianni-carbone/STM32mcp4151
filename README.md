# STM32mcp4151
Arduino STM32 library for microchip mcp 4151 digital potentiometer

## Description
Simple library with small foot-print for driving mcp 4xxx series microchip digital pots. The main objective of the library is to ensure maximum performance in the device update speed, at the expense of the availability of all functions deemed unnecessary. The functions for checking the status, reading and unary increments have not in fact been exported. On the other hand, the presence of a performing SPI and GPIO driver allows wiper updates in the order of 1.7 microseconds (with serial speed of 9 MHz). The standard driver, at the same speed, requires 35 microseconds for a single write.

## Principle of operation
The HAL APIs are used for initialization of SPI and GPIO devices. The writing of the peripherals is performed at the register level, bypassing all the unnecessary routines of the API calls of the arduino framework. Furthermore, the SPI writes are performed with a fixed datasize of 16 bits, instead of the two 8-bit calls operated by the arduino SPI.transfer16 function.
### pros
Updates that require only 1.7 microseconds (compared to 35 in the standard library) allow the implementation of direct digital synthesis capabilities of arbitrary waveforms and / or signal modulations at much higher frequencies.
### cons
Using a proprietary SPI driver prevents it from being shared with devices on the same bus with different clock rates or modes

In any case it is possible to configure the driver to use the standard SPI library and adopting transactions. In this way, however, maximum performance is lost.

## Examples
In the included example, the possibility of generating a sinusoidal waveform at the output frequency of 17.5KHz is demonstrated by adopting a sin table of 32 discrete points. Using the standard SPI driver, the same waveform would have been generated at just 800Hz.

## Notes
Although the datasheet of the device shows a maximum SPI clock frequency of 10MHz, bench tests have shown the possibility of using much higher frequencies (tests up to 36MHz). In such cases, however, glitches are generated quite often which affect the cleanliness of the output modulation.

### direct digital synthesis of sine wave using a sin table of 32 discrete points 
![STM32mcp4151_Sine32_17K5Hz](https://user-images.githubusercontent.com/16845473/159183637-dc90a8e4-43a9-4720-833e-06adc5a2b5a4.png)
