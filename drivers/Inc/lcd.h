/*
 * lcd.h
 *
 *  Created on: Aug 23, 2023
 *      Author: hoang
 */

#ifndef SRC_LCD_H_
#define SRC_LCD_H_
#include "stm32f411xx.h"
#include<stdint.h>
#include<string.h>

#define SLAVE_ADDRESS_LCD 0x27


void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);

void lcd_display_return_home_i2c(void);

void lcd_display_clear_i2c(void);







void lcd_init_i2c(void);
void lcd_send_command(uint8_t cmd);
void lcd_print_char(uint8_t data);
void lcd_display_clear(void);
void lcd_display_return_home(void);
void lcd_print_string(char*);
void lcd_set_cursor(uint8_t row, uint8_t column);

/*Application configurable items */

#define LCD_GPIO_PORT 		 	GPIOD


#define LCD_I2C2_PORT  			GPIOB

#define LCD_I2C2_SDA  			GPIO_PIN_NO_3
#define LCD_I2C2_SCL  			GPIO_PIN_NO_10
#define LCD_I2C2  				I2C2
#define LCD_I2C_SPEED 			I2C_SCL_SPEED_SM


/*LCD commands */
#define LCD_CMD_4DL_2N_5X8F  		0x28
#define LCD_CMD_DON_CURON    		0x0E
#define LCD_CMD_INCADD       		0x06
#define LCD_CMD_DIS_CLEAR    		0X01
#define LCD_CMD_DIS_RETURN_HOME  	0x02


#define LCD_GPIO_RS	   GPIO_PIN_NO_0
#define LCD_GPIO_RW	   GPIO_PIN_NO_1
#define LCD_GPIO_EN	   GPIO_PIN_NO_2
#define LCD_GPIO_D4	   GPIO_PIN_NO_3
#define LCD_GPIO_D5	   GPIO_PIN_NO_4
#define LCD_GPIO_D6	   GPIO_PIN_NO_5
#define LCD_GPIO_D7	   GPIO_PIN_NO_6
#endif /* SRC_LCD_H_ */
