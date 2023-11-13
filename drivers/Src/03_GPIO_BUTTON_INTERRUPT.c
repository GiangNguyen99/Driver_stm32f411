/*
 * 03_GPIO_BUTTON_INTERRUPT.c
 *
 *  Created on: Jul 31, 2023
 *      Author: hoang
 */

#include "stm32f411xx.h"
#define BTN_PRESS SET
void delay(void) {
	for (uint32_t var = 0; var < 500000 / 2; ++var) {
	}
}
int main(void) {

	GPIO_Handle_t GPIO_led1, GPIO_btn;
	//internal led 1: PD12
	GPIO_led1.pGPIOx = GPIOD;
	GPIO_led1.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_led1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_led1.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_led1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_led1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_PeriClockControl(GPIOD, ENABLE);
	GPIO_Init(&GPIO_led1);

	//external button PA1
	GPIO_btn.pGPIOx = GPIOA;
	GPIO_btn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIO_btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;
	GPIO_btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

	GPIO_PeriClockControl(GPIOA, ENABLE);
	GPIO_Init(&GPIO_btn);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI1, NVIC_IRQ_PRI_10);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI1, ENABLE);

	while (1);

	return 0;
}

void EXTI1_IRQHandler(void) {
	delay();
	GPIO_IRQHandling(GPIO_PIN_NO_1);
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
}

