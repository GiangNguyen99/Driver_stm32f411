/*
 * 08_I2C_receiveData_ArduinoToSTM32.c
 *
 *  Created on: Aug 3, 2023
 *      Author: hoang
 */

#include "stm32f411xx.h"
#include <string.h>
/*SEC58: EXERCISE
 *
 *
 * STMF4F11xx Discovery
 *
 * send data from stm32 to arduino mega, using internal resistor.
 * I2C1 - AF4
 *
 * 		PB6: SCL
 * 		PB7: SDA
 *
 *
 *
 */
extern void initialise_monitor_handles();
#define MY_ADDR 0x61;
#define SLAVE_ADDR  0x68		//COMPILED ARDUINO CODE TO TAKE SLAVE ADDRESS

I2C_Handle_t I2C1Handle;

//data receive
uint8_t data_buffer[500];

void I2C1_GPIOInits(void)
{
	GPIO_Handle_t I2CPins;

	/*Note : Internal pull-up resistors are used */

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	//scl
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_6;
	GPIO_PeriClockControl(GPIOB, ENABLE);

	GPIO_Init(&I2CPins);

	//sda
	//Note : since we found a glitch on PB9 , you can also try with PB7
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;

	GPIO_Init(&I2CPins);

}

void I2C1_Inits(void)
{
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR
	;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);

}
void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIO_btn, GpioLed;

	//this is btn gpio configuration

	//external button PA1
	GPIO_btn.pGPIOx = GPIOA;
	GPIO_btn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIO_btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_btn);

	//this is led gpio configuration
	GpioLed.pGPIOx = GPIOD;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_PeriClockControl(GPIOD, ENABLE);

	GPIO_Init(&GpioLed);

}

void delay(void)
{
	for (uint32_t i = 0; i < 500000 / 2; i++)
		;
}
int main(void)
{

	uint8_t commandcode;

	uint8_t len;

//	initialise_monitor_handles();

//	printf("Application is running\n");

	GPIO_ButtonInit();

	//i2c pin inits
	I2C1_GPIOInits();

	//i2c peripheral configuration
	I2C1_Inits();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//ack bit is made 1 after PE=1
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while (1)
	{
		//wait till button is pressed
		while (!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1))
			;

		//to avoid button de-bouncing related issues 200ms of delay
		delay();
//		printf("Application is running...\n");
		commandcode = 0x51;

		I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR,
		I2C_ENABLE_SR);

		I2C_MasterReceiveData(&I2C1Handle, &len, 1, SLAVE_ADDR, I2C_ENABLE_SR);

		commandcode = 0x52;
		I2C_MasterSendData(&I2C1Handle, &commandcode, 1, SLAVE_ADDR,
		I2C_ENABLE_SR);

		I2C_MasterReceiveData(&I2C1Handle, data_buffer, len, SLAVE_ADDR,
		I2C_DISABLE_SR);

		data_buffer[len + 1] = '\0';

//		printf("Data : %s",data_buffer);
	}
	return 0;
}
