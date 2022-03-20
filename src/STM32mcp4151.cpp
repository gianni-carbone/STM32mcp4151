#include "STM32mcp4151.h"

#ifndef USE_HAL_SPI	
#include <SPI.h>	
#endif

#ifdef USE_HAL_GPIO
#define cs_set()   st.csGpioPort->ODR |= st.csGpioPin
#define cs_reset() st.csGpioPort->ODR &= ~st.csGpioPin
#else
#define cs_set()   digitalWrite(st.csPin, HIGH)
#define cs_reset() digitalWrite(st.csPin, LOW)
#endif




#ifdef USE_HAL_SPI	

#define cs_delay()			asm("NOP")	
#define chip_select()		cs_reset(); cs_delay()
#define chip_deselect()		cs_set()

#define spi_wait_tx_empty() while(!(st.hspi.Instance->SR  & SPI_FLAG_TXE))
#define spi_wait_not_busy()	while ((st.hspi.Instance->SR  & SPI_FLAG_BSY))

#define spi_wait_rdy()		spi_wait_tx_empty(); spi_wait_not_busy()
#define spi_send(d)			cs_reset(); cs_delay(); st.hspi.Instance->DR = (d); spi_wait_rdy(); cs_set()
#define spi_send_cont(d)	spi_wait_tx_empty(); st.hspi.Instance->DR = (d);
	
#else

#define chip_select()		cs_reset()
#define chip_deselect()		cs_set()
#define spi_send(d)			SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE3)); chip_select(); SPI.transfer16((d)); chip_deselect(); SPI.endTransaction(); 
#define spi_send_cont(d)	spi_send(d); 
#endif


#ifdef USE_HAL_SPI		
STM32mcp4151::STM32mcp4151(uint32_t cs_pin, SPI_TypeDef* SPIinstance, bool initSPI)
#else
STM32mcp4151::STM32mcp4151(uint32_t cs_pin, bool initSPI)	
#endif
{
	st.csPin = cs_pin;
	
	if (initSPI) {
#ifdef USE_HAL_SPI		
		st.hspi.Instance = SPIinstance;	
		SPI_MspInit();
		SPI_Init();			// TODO: manage returns
#else
		SPI.begin(); 
#endif
	}
	
#ifdef USE_HAL_GPIO	
	PinName pn = digitalPinToPinName(st.csPin);
	st.csGpioPort = get_GPIO_Port(STM_PORT(pn));
	st.csGpioPin = STM_GPIO_PIN(pn);
	GPIO_Init();
#else
	pinMode(st.csPin, OUTPUT);
#endif
}

uint32_t STM32mcp4151::version(void) {
	return STM32mcp4151_VER;
}

void STM32mcp4151::continuos(bool c) {
	st.continuos = c;
	if (st.continuos) {
		chip_deselect();
		chip_select();
	} else {
		chip_deselect();
	}
}

void STM32mcp4151::wiper(uint16_t w) {
	if (st.continuos) {
		spi_send_cont(((wiper_addr | write_cmd | (w)>>8) <<8) | (byte)(w)&0x1FF);
	} else{
		spi_send(((wiper_addr | write_cmd | (w)>>8) <<8) | (byte)(w)&0x1FF);
	}
}

// Private
#ifdef USE_HAL_SPI	

bool STM32mcp4151::SPI_MspInit(){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	// assuming PinMap_SPI_MISO, PinMap_SPI_MOSI and PinMap_SPI_SCLK same size
	uint8_t i=0;
	while (PinMap_SPI_MISO[i].pin != NC) {
		
		if (st.hspi.Instance == PinMap_SPI_MISO[i].peripheral) {
			GPIO_InitStruct.Pin = STM_GPIO_PIN(PinMap_SPI_MISO[i].pin);
			HAL_GPIO_Init(get_GPIO_Port(STM_PORT(PinMap_SPI_MISO[i].pin)), &GPIO_InitStruct);
			set_GPIO_Port_Clock(STM_PORT(PinMap_SPI_MISO[i].pin));
			GPIO_InitStruct.Pin = STM_GPIO_PIN(PinMap_SPI_MOSI[i].pin);
			HAL_GPIO_Init(get_GPIO_Port(STM_PORT(PinMap_SPI_MOSI[i].pin)), &GPIO_InitStruct);
			set_GPIO_Port_Clock(STM_PORT(PinMap_SPI_MOSI[i].pin));
			GPIO_InitStruct.Pin = STM_GPIO_PIN(PinMap_SPI_SCLK[i].pin);
			HAL_GPIO_Init(get_GPIO_Port(STM_PORT(PinMap_SPI_SCLK[i].pin)), &GPIO_InitStruct);
			set_GPIO_Port_Clock(STM_PORT(PinMap_SPI_SCLK[i].pin));
		}
		i++;
	}

	return true;
}


bool STM32mcp4151::SPI_Init(void){
	// enable clock here is mandatory
#ifdef SPI1	
	if(st.hspi.Instance == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
#endif
#ifdef SPI2	
	if(st.hspi.Instance == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
#endif
#ifdef SPI3	
	if(st.hspi.Instance == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();
#endif
#ifdef SPI4	
	if(st.hspi.Instance == SPI4) __HAL_RCC_SPI4_CLK_ENABLE();
#endif

	st.hspi.Init.Mode = SPI_MODE_MASTER;
	st.hspi.Init.Direction = SPI_DIRECTION_2LINES;
	st.hspi.Init.CLKPhase = SPI_PHASE_2EDGE;					// SPI MODE3
	st.hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;				// SPI MODE3
	st.hspi.Init.DataSize = SPI_DATASIZE_16BIT;					// always write 16 bit of data
	st.hspi.Init.NSS = SPI_NSS_SOFT;
	st.hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;	// SPI1: prescaler = 8 tested with STM32F103@72MHz. Work to 36MHz with some glitches
	st.hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
	st.hspi.Init.TIMode = SPI_TIMODE_DISABLE;
	st.hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	st.hspi.Init.CRCPolynomial = 7;

	bool ok = (HAL_SPI_Init(&st.hspi) == HAL_OK);
	
	if (ok) 
		__HAL_SPI_ENABLE(&st.hspi);                        // MANDATORY

  return ok;	
}
#endif

#ifdef USE_HAL_GPIO
void STM32mcp4151::GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// enable GPIO port clock for csPin
	PinName pn = digitalPinToPinName(st.csPin);
	set_GPIO_Port_Clock(STM_PORT(pn));

	HAL_GPIO_WritePin(st.csGpioPort, st.csGpioPin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = st.csGpioPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(st.csGpioPort, &GPIO_InitStruct);
}
#endif