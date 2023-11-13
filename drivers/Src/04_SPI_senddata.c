/*
 * 04_SPI_senddata.c
 *
 *  Created on: Aug 1, 2023
 *      Author: hoang
 */

#include "stm32f411xx.h"
#include <string.h>

/*		SPI1
 * 			PA4: NSS
 * 			PA5: SCLK
 * 			PA6: MISO
 * 			PA7: MOSI
 * 		AF5
 *
 *
 *
 *
 * */
void SPI1_GPIOInits(void) {
	GPIO_Handle_t SPIPins;

	SPIPins.pGPIOx = GPIOA;
	SPIPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	SPIPins.GPIO_PinConfig.GPIO_PinAltFunMode = 5;
	SPIPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	SPIPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//SCLK
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&SPIPins);

	//MOSI
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&SPIPins);

	//MISO
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_14;
	//GPIO_Init(&SPIPins);

	//NSS
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	//GPIO_Init(&SPIPins);

}

void SPI1_Inits(void) {

	SPI_Handle_t SPI1handle;

	SPI1handle.pSPIx = SPI1;
	SPI1handle.SPI_Config.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI1handle.SPI_Config.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1handle.SPI_Config.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV2;//generates sclk of 8MHz
	SPI1handle.SPI_Config.SPI_DFF = SPI_DFF_8BITS;
	SPI1handle.SPI_Config.SPI_CPOL = SPI_CPOL_HIGH;
	SPI1handle.SPI_Config.SPI_CPHA = SPI_CPHA_LOW;
	SPI1handle.SPI_Config.SPI_SSM = SPI_SSM_EN; //software slave management enabled for NSS pin

	SPI_Init(&SPI1handle);
}

void delay(void) {
	for (uint32_t var = 0; var < 50000; ++var) {
	}
}
int main(void) {

	char user_data[] = "Hello world";
	SPI1_GPIOInits();
	SPI1_Inits();

	//this makes NSS signal internally high and avoids MODF error
	SPI_SSIConfig(SPI1, ENABLE);

	SPI_PeripheralControl(SPI1, ENABLE);
	while(1)
	{
	SPI_SendData(SPI1, (uint8_t*) user_data, strlen(user_data));

	//lets confirm SPI1 is not busy
	while (SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG))
		;
	delay();
	}
	//Disable the SPI1 peripheral
	SPI_PeripheralControl(SPI1, DISABLE);
	return 0;
}
