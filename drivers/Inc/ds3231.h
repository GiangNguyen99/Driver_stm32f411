/*
 * ds3231.h
 *
 *  Created on: Aug 23, 2023
 *      Author: hoang
 */
#include "stm32f411xx.h"

#ifndef DS3231_H_
#define DS3231_H_


#define DS3231_I2C_ADDRESS 0x68

/*Register addresses */
#define DS3231_ADDR_SEC 		0x00
#define DS3231_ADDR_MIN 		0x01
#define DS3231_ADDR_HRS			0x02
#define DS3231_ADDR_DAY			0x03
#define DS3231_ADDR_DATE		0x04
#define DS3231_ADDR_MONTH		0x05
#define DS3231_ADDR_YEAR		0x06
#define DS3231_ADDR_SEC_AL1 	0x07
#define DS3231_ADDR_MIN_AL1 	0x08
#define DS3231_ADDR_HRS_AL1		0x09
#define DS3231_ADDR_DYDT_AL1	0x0A
#define DS3231_ADDR_MIN_AL2 	0x0B
#define DS3231_ADDR_HRS_AL2		0x0C
#define DS3231_ADDR_DYDT_AL2	0x0D

#define DS3231_ADDR_CR 			0x0E		//Control Register
#define DS3231_ADDR_SR			0x0F		//Status Register
#define DS3231_ADDR_AO			0x10		//Aging Offset
#define DS3231_ADDR_TR_UB		0x11		//Temperature Register (Upper Byte)
#define DS3231_ADDR_TR_LB		0x12		//Temperature Register (Lower Byte)



/*Application configurable items */
#define DS3231_I2C  			I2C1
#define DS3231_I2C_GPIO_PORT    GPIOB
#define DS3231_I2C_SDA_PIN 		GPIO_PIN_NO_7
#define DS3231_I2C_SCL_PIN 		GPIO_PIN_NO_6
#define DS3231_I2C_SPEED 		I2C_SCL_SPEED_SM
//#define DS3231_I2C_SPEED 		I2C_SCL_SPEED_FM4K

#define DS3231_I2C_PUPD			GPIO_PIN_PU


#define TIME_FORMAT_12HRS_AM 	0
#define TIME_FORMAT_12HRS_PM 	1
#define TIME_FORMAT_24HRS 		2


#define SUNDAY  	1;
#define MONDAY  	2;
#define TUESDAY  	3;
#define WEDNESDAY   4;
#define THURSDAY  	5;
#define FRIDAY  	6;
#define SATURDAY  	7;


/*Application configurable items */

#define DS3231_CR_DI_AL12  	0x1C
#define DS3231_CR_EN_AL1  	0x1D
#define DS3231_CR_EN_AL2  	0x1E
#define DS3231_CR_EN_AL12  	0x1F


#define DS3231_MB_AL1_0  	0x0F
#define DS3231_MB_AL1_1  	0x0E
#define DS3231_MB_AL1_2  	0x0C
#define DS3231_MB_AL1_3  	0x08
#define DS3231_MB_AL1_4  	0x00
#define DS3231_MB_AL1_5  	0x10


typedef struct
{
	uint8_t date;
	uint8_t month;
	uint8_t year;
	uint8_t day;
}RTC_date_t;


typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t time_format;
}RTC_time_t;



//Function prototypes

uint8_t ds3231_init(void);

void ds3231_set_current_time(RTC_time_t *);
void ds3231_get_current_time(RTC_time_t *);

void ds3231_set_current_date(RTC_date_t *);
void ds3231_get_current_date(RTC_date_t *);
void ds3231_set_time_alarm1(RTC_time_t *rtc_time, RTC_date_t *rtc_date, uint8_t mode_alarm);
void ds3231_clear_sr(void);

#endif /* DS3231_H_ */
