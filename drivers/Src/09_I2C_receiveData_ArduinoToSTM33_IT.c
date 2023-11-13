/*
 * 09_I2C_receiveData_ArduinoToSTM32_IT.c
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

#define MY_ADDR 0x61;
#define SLAVE_ADDR  0x68		//COMPILED ARDUINO CODE TO TAKE SLAVE ADDRESS

I2C_Handle_t I2C1Handle;

//data receive
uint8_t data_buffer[32];

//Flag variable
uint8_t rxComplt = RESET;

void I2C1_GPIOInits(void) {
	GPIO_Handle_t I2CPins;

	/*Note : Internal pull-up resistors are used */

	I2CPins.pGPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
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

void I2C1_Inits(void) {
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR
	;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);

}
void GPIO_ButtonInit(void) {
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

void delay(void) {
	for (uint32_t i = 0; i < 500000 / 2; i++)
		;
}
int main(void) {

	uint8_t commandcode;

	uint8_t len;

	GPIO_ButtonInit();

	//i2c pin inits
	I2C1_GPIOInits();

	//i2c peripheral configuration
	I2C1_Inits();

	//I2C IRQ configurations
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//ack bit is made 1 after PE=1
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	while (1) {
		//wait till button is pressed
		while (!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1))
			;

		//to avoid button de-bouncing related issues 200ms of delay
		delay();

		commandcode = 0x51;

		while (I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR,
		I2C_ENABLE_SR) != I2C_READY)
			;

		while (I2C_MasterReceiveDataIT(&I2C1Handle, &len, 1, SLAVE_ADDR,
		I2C_ENABLE_SR) != I2C_READY)
			;

		commandcode = 0x52;
		while (I2C_MasterSendDataIT(&I2C1Handle, &commandcode, 1, SLAVE_ADDR,
		I2C_ENABLE_SR) != I2C_READY)
			;

		while (I2C_MasterReceiveDataIT(&I2C1Handle, data_buffer, len,
				SLAVE_ADDR, I2C_DISABLE_SR) != I2C_READY)
			;

		rxComplt = RESET;

		//wait till rx completes
		while (rxComplt != SET) {

		}

		data_buffer[len + 1] = '\0';

		printf("Data : %s", data_buffer);

		rxComplt = RESET;
	}
	return 0;
}

void I2C1_EV_IRQHandler(void) {
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void) {
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv) {
	if (AppEv == I2C_EV_TX_CMPLT) {
		printf("Tx is completed\n");
	} else if (AppEv == I2C_EV_RX_CMPLT) {
		printf("Rx is completed\n");
		rxComplt = SET;
	} else if (AppEv == I2C_ERROR_AF) {
		printf("Error : Ack failure\n");
		//in master ack failure happens when slave fails to send ack for the byte
		//sent from the master.
		I2C_CloseSendData(pI2CHandle);

		//generate the stop condition to release the bus
		I2C_GenerateStopCondition(I2C1);

		//Hang in infinite loop
		while (1)
			;
	}
}
