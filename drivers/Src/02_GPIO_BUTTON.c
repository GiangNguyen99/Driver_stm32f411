/*
 *  02_GPIO_BUTTON.c
 *
 *  Created on: Jul 30, 2023
 *      Author: hoang
 *
 *   dung nut nhan ngoai bat tat 2 led co san tren board
 *	 note:
 *	 Vdd -> pin 1 (button); pin 2 (button) -> PA1 (stm32)
 */

#include "stm32f411xx.h"
#define BTN_PRESS SET
void delay(void) {
	for (uint32_t var = 0; var < 500000 / 2; ++var) {
	}
}
int main(void) {

	GPIO_Handle_t GPIO_led1, GPIO_led2, GPIO_btn;
	uint8_t state = 0;
	//internal led 1: PD12
	GPIO_led1.pGPIOx = GPIOD;
	GPIO_led1.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_led1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_led1.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_led1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_led1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_led1);

	//internal led 2: PD13
	GPIO_led2.pGPIOx = GPIOD;
	GPIO_led2.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	GPIO_led2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_led2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_led2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_led2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_led2);

	//external button PA1
	GPIO_btn.pGPIOx = GPIOA;
	GPIO_btn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIO_btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIO_btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_btn);
	while (1) {
		if (GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1) == BTN_PRESS) {
			if (state == 0) {
				delay();
				GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
				state++;
			} else {
				delay();

				GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_13);
				state = 0;
			}
		}
	}

	return 0;
}
