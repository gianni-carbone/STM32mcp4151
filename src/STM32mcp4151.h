#ifndef STM32mcp4151_h
#define STM32mcp4151_h

#define USE_HAL_GPIO  	// enable the fast SFY pin toggle. Suggestion: stay it enabled
#define USE_HAL_SPI   	// enable the fastest SPI write. 
						// Suggestion: if the spi interface is for the exclusive use of the module then stay it enabled.
						// If you have to share the spi interface with other slaves with different settings than MODE3 @MCLK/8
						// then comment the line and use spi transactions.

#include <Arduino.h>
#include "stm32yyxx_hal_conf.h"

#define STM32mcp4151_VER	901

#define wiper_addr 			0x00
#define write_cmd  			0x00
#define increment_cmd  		0x04
#define decrement_cmd  		0x08

typedef struct {
	uint32_t		csPin = 0;
	bool			continuos = false;
	uint16_t		wiper = 0;

#ifdef USE_HAL_SPI	
	SPI_HandleTypeDef	hspi;
#endif
	
#ifdef USE_HAL_GPIO	
	GPIO_TypeDef*	csGpioPort = {0};
	uint16_t		csGpioPin = 0;
#endif

} mcp4151_status_t;

class STM32mcp4151 {
	
	public:
#ifdef USE_HAL_SPI		
	STM32mcp4151(uint32_t _cs_pin, SPI_TypeDef *SPIinstance = SPI1, bool initSPI = true);
#else
	STM32mcp4151(uint32_t _cs_pin, bool initSPI = true);
#endif
	uint32_t	version();
	void		continuos(bool c);	 			// set in continuos mode
	void		wiper(uint16_t w);	 
	
	private:
	mcp4151_status_t st;
#ifdef USE_HAL_SPI
	bool SPI_MspInit(void);
	bool SPI_Init(void);
#endif
#ifdef USE_HAL_GPIO
	void 			GPIO_Init(void);
#endif	
};

#endif