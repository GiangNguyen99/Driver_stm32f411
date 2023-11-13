/*
 * 05_SPI_sendData_STM32toArduinoMega.c
 *
 *  Created on: Aug 1, 2023
 *      Author: hoang
 *      Send data from STM32 to Arduino Mega 2560 when press button
 */

#include "stm32f411xx.h"
#include<string.h>

/*
 * stm32f411 discovery
 * 		SPI1
 * 			PA4: NSS
 * 			PA5: SCLK
 * 			PA6: MISO
 * 			PA7: MOSI
 * 		AF5
 *
 * arduino uno r3
 * 		SPI Slave Demo
 *
 * SPI pin numbers:
 * SCK   13  // Serial Clock.
 * MISO  12  // Master In Slave Out.
 * MOSI  11  // Master Out Slave In.
 * SS    10  // Slave Select . Arduino SPI pins respond only if SS pulled low by the master
 *
 *
 *
 *
 * */
void SPI1_GPIOInits(void)
{
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
	//SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	//GPIO_Init(&SPIPins);

	//NSS
	SPIPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_4;
	GPIO_Init(&SPIPins);

}

void SPI1_Inits(void)
{

	SPI_Handle_t SPI1handle;

	SPI1handle.pSPIx = SPI1;
	SPI1handle.SPI_Config.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI1handle.SPI_Config.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1handle.SPI_Config.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV8;//generates sclk of 2MHz
	SPI1handle.SPI_Config.SPI_DFF = SPI_DFF_8BITS;
	SPI1handle.SPI_Config.SPI_CPOL = SPI_CPOL_HIGH;
	SPI1handle.SPI_Config.SPI_CPHA = SPI_CPHA_HIGH;
	SPI1handle.SPI_Config.SPI_SSM = SPI_SSM_DI; //Hardware slave management enabled for NSS pin

	SPI_Init(&SPI1handle);
}

void GPIO_ButtonInit(void)
{

	GPIO_Handle_t GPIO_btn;

	//external button PA1
	GPIO_btn.pGPIOx = GPIOA;
	GPIO_btn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIO_btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_btn);
}

void delay(void)
{
	for (uint32_t var = 0; var < 300000; ++var)
	{
	}
}
int main(void)
{

	char user_data[] = "Hello world ";
	SPI1_GPIOInits();
	SPI1_Inits();
	GPIO_ButtonInit();
	/*
	 * making SSOE 1 does NSS output enable.
	 * The NSS pin is automatically managed by the hardware.
	 * i.e when SPE=1 , NSS will be pulled to low
	 * and NSS pin will be high when SPE=0
	 */
	SPI_SSOEConfig(SPI1, ENABLE);

	while (1)
	{
		//wait till button is pressed
		while (!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1))
			;

		//to avoid button de-bouncing related issues 200ms of delay
		delay();
		SPI_PeripheralControl(SPI1, ENABLE);

		//first send length information
		uint8_t dataLen = strlen(user_data);
		SPI_SendData(SPI1, &dataLen, 1);

		SPI_SendData(SPI1, (uint8_t*) user_data, strlen(user_data));

		//lets confirm SPI is not busy
		while (SPI_GetFlagStatus(SPI1, SPI_BUSY_FLAG))
			;

		//Disable the SPI1 peripheral
		SPI_PeripheralControl(SPI1, DISABLE);
	}
	return 0;
}

