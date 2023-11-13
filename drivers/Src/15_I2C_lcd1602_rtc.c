/*
 * 15_lcd1602_4bit.c
 *
 *  Created on: Aug 24, 2023
 *      Author: hoang
 */
/*	stm32 + lcd1602_i2c + rtc_ds3231
 * 	- lcd1602_i2c
 * 		+ I2C2_SCL ==> PB10
 * 		+ I2C2_SDA ==> PB3
 * 	- rtc_ds3231
 * 		+ I2C1_SCL ==> PB6
 * 		+ I2C1_SDA ==> PB7
 * 	- GPIO_alarm
 * 		+ PA3
 * 	- GPIO_LED
 * 		+ PD12
 *
 *
 *
 * */
#include<stdio.h>
#include<string.h>
#include "stm32f411xx.h"
#include "ds3231.h"

#define SYSTICK_TIM_CLK   16000000UL

//I2C_Handle_t hi2c2;

/* Enable this macro if you want to test RTC on LCD */
#define PRINT_LCD

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t *pSRVR = (uint32_t*) 0xE000E014;
	uint32_t *pSCSR = (uint32_t*) 0xE000E010;

	/* calculation of reload value */
	uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1;

	//Clear the value of SVR
	*pSRVR &= ~(0x00FFFFFFFF);

	//load the value in to SVR
	*pSRVR |= count_value;

	//do some settings
	*pSCSR |= (1 << 1); //Enables SysTick exception request:
	*pSCSR |= (1 << 2);  //Indicates the clock source, processor clock source

	//enable the systick
	*pSCSR |= (1 << 0); //enables the counter

}

char* get_day_of_week(uint8_t i)
{
	char *days[] =
	{ "Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat" };

	return days[i - 1];
}

void number_to_string(uint8_t num, char *buf)
{

	if (num < 10)
	{
		buf[0] = '0';
		buf[1] = num + 48;
	}
	else if (num >= 10 && num < 99)
	{
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
	}
}

//hh:mm:ss
char* time_to_string(RTC_time_t *rtc_time)
{
	static char buf[9];

	buf[2] = ':';
	buf[5] = ':';

	number_to_string(rtc_time->hours, buf);
	number_to_string(rtc_time->minutes, &buf[3]);
	number_to_string(rtc_time->seconds, &buf[6]);

	buf[8] = '\0';

	return buf;

}

//dd/mm/yy
char* date_to_string(RTC_date_t *rtc_date)
{
	static char buf[9];

	buf[2] = '/';
	buf[5] = '/';

	number_to_string(rtc_date->date, buf);
	number_to_string(rtc_date->month, &buf[3]);
	number_to_string(rtc_date->year, &buf[6]);

	buf[8] = '\0';

	return buf;

}

static void mdelay(uint32_t cnt)
{
	for (uint32_t i = 0; i < (cnt * 1000); i++)
		;
}

void GPIO_ButtonInit(void)
{
	GPIO_Handle_t GPIOBtn;

	//this is btn gpio configuration
	GPIOBtn.pGPIOx = GPIOA;
	GPIOBtn.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_1;
	GPIOBtn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	GPIOBtn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIOBtn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;

	GPIO_Init(&GPIOBtn);

}

void GPIO_alarmInit(void)
{
	GPIO_Handle_t GPIO_led1, GPIO_al;
	//internal led 1: PD12
	GPIO_led1.pGPIOx = GPIOD;
	GPIO_led1.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_12;
	GPIO_led1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GPIO_led1.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_led1.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GPIO_led1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_Init(&GPIO_led1);

	//external alarm PA3
	GPIO_al.pGPIOx = GPIOA;
	GPIO_al.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_al.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
	GPIO_al.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_al.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;

	GPIO_Init(&GPIO_al);

	GPIO_IRQPriorityConfig(IRQ_NO_EXTI3, NVIC_IRQ_PRI_1);
	GPIO_IRQInterruptConfig(IRQ_NO_EXTI3, ENABLE);

}

void EXTI3_IRQHandler(void)
{
	mdelay(5);
	GPIO_IRQHandling(GPIO_PIN_NO_3);
	GPIO_ToggleOutputPin(GPIOD, GPIO_PIN_NO_12);
	ds3231_clear_sr();

}
int main(void)
{

	RTC_time_t current_time;
	RTC_date_t current_date;

	RTC_time_t time_alarm1;
	RTC_date_t date_alarm1;

//	GPIO_ButtonInit();

//	while (1)
//	{
//		//wait till button is pressed
//		while (!GPIO_ReadFromInputPin(GPIOA, GPIO_PIN_NO_1))
//			;
//		mdelay(200);
	lcd_init_i2c();

#ifndef PRINT_LCD
	printf("RTC test\n");
#else

#endif

	lcd_display_clear_i2c();

	lcd_display_return_home_i2c();
	if (ds3231_init())
	{
		printf("RTC init has failed\n");
		while (1)
			;
	}

	current_date.day = FRIDAY
	;
	current_date.date = 25;
	current_date.month =8;
	current_date.year = 23;

	current_time.hours = 11;
	current_time.minutes = 59;
	current_time.seconds = 30;
	current_time.time_format = TIME_FORMAT_12HRS_PM;

	time_alarm1.hours = 11;
	time_alarm1.minutes = 59;
	time_alarm1.seconds = 40;
	time_alarm1.time_format = TIME_FORMAT_12HRS_PM;
	date_alarm1.day = FRIDAY
	;

	ds3231_set_current_date(&current_date);
	ds3231_set_current_time(&current_time);
	ds3231_set_time_alarm1(&time_alarm1, &date_alarm1, DS3231_MB_AL1_5);

	ds3231_get_current_time(&current_time);
	ds3231_get_current_date(&current_date);

	GPIO_alarmInit();
	init_systick_timer(1);
	char *am_pm;
	if (current_time.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = (current_time.time_format) ? "PM" : "AM";
#ifndef PRINT_LCD
		printf("Current time = %s %s\n",time_to_string(&current_time),am_pm); // 04:25:41 PM
#else
//		lcd_print_string(time_to_string(&current_time));
//		lcd_print_string(am_pm);

		lcd_send_string(time_to_string(&current_time));
		lcd_send_string(am_pm);
#endif
	}
	else
	{
#ifndef PRINT_LCD
		printf("Current time = %s\n",time_to_string(&current_time)); // 04:25:41
#else
//		lcd_print_string(time_to_string(&current_time));
		lcd_send_string(time_to_string(&current_time));
#endif
	}

#ifndef PRINT_LCD
	printf("Current date = %s <%s>\n",date_to_string(&current_date), get_day_of_week(current_date.day));
#else
//	lcd_set_cursor(2, 1);
//	lcd_print_string(date_to_string(&current_date));
	lcd_put_cur(2, 1);
	lcd_send_string(date_to_string(&current_date));

#endif
	while (1)
		;

	return 0;
}

void SysTick_Handler(void)
{
	RTC_time_t current_time;
	RTC_date_t current_date;

	ds3231_get_current_time(&current_time);

	char *am_pm;
	if (current_time.time_format != TIME_FORMAT_24HRS)
	{
		am_pm = (current_time.time_format) ? "PM" : "AM";
#ifndef PRINT_LCD
		printf("Current time = %s %s\n",time_to_string(&current_time),am_pm); // 04:25:41 PM
#else
		lcd_put_cur(1, 1);
		lcd_send_string(time_to_string(&current_time));
		lcd_send_string(am_pm);
#endif

	}
	else
	{
#ifndef PRINT_LCD
		printf("Current time = %s\n",time_to_string(&current_time)); // 04:25:41
#else
		lcd_put_cur(1, 1);
		lcd_send_string(time_to_string(&current_time));
#endif
	}

	ds3231_get_current_date(&current_date);

#ifndef PRINT_LCD
	printf("Current date = %s <%s>\n",date_to_string(&current_date), get_day_of_week(current_date.day));
#else
	lcd_put_cur(2, 1);
	lcd_send_string(date_to_string(&current_date));
	lcd_send_data('<');
	lcd_send_string(get_day_of_week(current_date.day));
	lcd_send_data('>');
#endif

}
