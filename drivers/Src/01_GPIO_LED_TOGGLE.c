/*
 * 01_led_toggle.c
 *
 *  Created on: Jul 30, 2023
 *      Author: hoang
 */

#include "stm32f411xx.h"

void delay(void) {
	for (uint32_t var = 0; var < 500000; ++var) {
	}
}
int main(void) {
	GPIO_Handle_t GPIO_led;
	GPIO_led.pGPIOx = GPIOA;
	GPIO_led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_led);

	GPIO_led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GPIO_Init(&GPIO_led);

	GPIO_led.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_7;
	GPIO_Init(&GPIO_led);

	while (1) {
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_3);
		delay();
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
		delay();
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_7);
		delay();
	}

	return 0;
}
