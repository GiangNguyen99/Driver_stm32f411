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
 * receive command from Arduino(master) and transmitter data from stm32(slave)
 * I2C1 - AF4
 *
 * 		PB6: SCL
 * 		PB7: SDA
 *
 *
 *
 */

I2C_Handle_t I2C1Handle;

#define MY_ADDR 0x68;
uint32_t data_len = 0;
#define SLAVE_ADDR  0x68

//very large message
uint8_t Tx_buf[] =
		"HiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHi...123";
//rcv buffer
uint8_t rcv_buf[32];
uint8_t CommandCode;

void I2C1_GPIOInits(void) {
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

void I2C1_Inits(void) {
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = SLAVE_ADDR
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

	data_len = strlen((char*) Tx_buf);

	GPIO_ButtonInit();

	//i2c pin inits
	I2C1_GPIOInits();

	//i2c peripheral configuration
	I2C1_Inits();

	//enable the i2c peripheral
	I2C_PeripheralControl(I2C1, ENABLE);

	//ack bit is made 1 after PE=1
	I2C_ManageAcking(I2C1, I2C_ACK_ENABLE);

	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	while (1)
		;

	return 0;
}

void I2C1_EV_IRQHandler(void) {
	I2C_EV_IRQHandling(&I2C1Handle);
}

void I2C1_ER_IRQHandler(void) {
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv) {

	static uint32_t cnt = 0;
	static uint32_t w_ptr = 0;

	if (AppEv == I2C_ERROR_AF) {
		//This will happen during slave transmitting data to master .
		// slave should understand master needs no more data
		//slave concludes end of Tx

		//if the current active code is 0x52 then dont invalidate
		if (!(CommandCode == 0x52))
			CommandCode = 0XFF;

		//reset the cnt variable because its end of transmission
		cnt = 0;

		//Slave concludes it sent all the bytes when w_ptr reaches data_len
		if (w_ptr >= (data_len)) {
			w_ptr = 0;
			CommandCode = 0xff;
		}

	} else if (AppEv == I2C_EV_STOP) {
		//This will happen during end slave reception
		//slave concludes end of Rx

		cnt = 0;

	} else if (AppEv == I2C_EV_DATA_REQ) {
		//Master is requesting for the data . send data
		if (CommandCode == 0x51) {
			//Here we are sending 4 bytes of length information
			I2C_SlaveSendData(I2C1, ((data_len >> ((cnt % 4) * 8)) & 0xFF));
			cnt++;
		} else if (CommandCode == 0x52) {
			//sending Tx_buf contents indexed by w_ptr variable
			I2C_SlaveSendData(I2C1, Tx_buf[w_ptr++]);
		}
	} else if (AppEv == I2C_EV_DATA_RCV) {
		//Master has sent command code, read it
		CommandCode = I2C_SlaveReceiveData(I2C1);

	}
}
