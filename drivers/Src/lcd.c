/*
 * lcd.c
 *
 *  Created on: Aug 23, 2023
 *      Author: hoang
 */

#include "lcd.h"
#include "stm32f411xx_i2c_driver.h"

I2C_Handle_t hi2c2;

static void write_4_bits(uint8_t value);
static void lcd_enable(void);
static void mdelay(uint32_t cnt);
static void udelay(uint32_t cnt);

void lcd_send_command(uint8_t cmd)
{
	/* RS=0 for LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/*R/nW = 0, for write */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(cmd >> 4);
	write_4_bits(cmd & 0x0F);

}

/*
 *This function sends a character to the LCD
 *Here we used 4 bit parallel data transmission.
 *First higher nibble of the data will be sent on to the data lines D4,D5,D6,D7
 *Then lower nibble of the data will be set on to the data lines D4,D5,D6,D7
 */
void lcd_print_char(uint8_t data)
{
	/* RS=1 for LCD user data */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_SET);

	/*R/nW = 0, for write */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(data >> 4); /*Higher nibble*/
	write_4_bits(data & 0x0F); /*Lower nibble*/

}

void lcd_print_string(char *message)
{

	do
	{
		lcd_print_char((uint8_t) *message++);
	} while (*message != '\0');

}

void lcd_init(void)
{

	//1. Configure the gpio pins which are used for lcd connections

	GPIO_Handle_t lcd_signal;

	lcd_signal.pGPIOx = LCD_GPIO_PORT;
	lcd_signal.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	lcd_signal.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	lcd_signal.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	lcd_signal.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RW;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_RS;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_EN;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D4;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D5;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D6;
	GPIO_Init(&lcd_signal);

	lcd_signal.GPIO_PinConfig.GPIO_PinNumber = LCD_GPIO_D7;
	GPIO_Init(&lcd_signal);

	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

	//2. Do the LCD initialization

	mdelay(40);

	/*RS = 0 , For LCD command */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	/* RnW = 0, Writing to LCD */
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);

	write_4_bits(0x3);

	mdelay(5);

	write_4_bits(0x3);

	udelay(150);

	write_4_bits(0x3);
	write_4_bits(0x2);

	//function set command
	lcd_send_command(LCD_CMD_4DL_2N_5X8F);

	//disply ON and cursor ON
	lcd_send_command(LCD_CMD_DON_CURON);

	lcd_display_clear();

	//entry mode set
	lcd_send_command(LCD_CMD_INCADD);

}

/* writes 4 bits of data/command on to D4,D5,D6,D7 lines */
static void write_4_bits(uint8_t value)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D4, ((value >> 0) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D5, ((value >> 1) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D6, ((value >> 2) & 0x1));
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_D7, ((value >> 3) & 0x1));

	lcd_enable();

}
void lcd_display_clear(void)
{
	//Display clear
	lcd_send_command(LCD_CMD_DIS_CLEAR);

	/*
	 * check page number 24 of datasheet.
	 * display clear command execution wait time is around 2ms
	 */

	mdelay(2);
}
void lcd_display_clear_i2c(void)
{
	//Display clear
	lcd_send_cmd(LCD_CMD_DIS_CLEAR);

	/*
	 * check page number 24 of datasheet.
	 * display clear command execution wait time is around 2ms
	 */

	mdelay(2);
}

/*Cursor returns to home position */
void lcd_display_return_home(void)
{

	lcd_send_cmd(LCD_CMD_DIS_RETURN_HOME);
	/*
	 * check page number 24 of datasheet.
	 * return home command execution wait time is around 2ms
	 */
	mdelay(2);
}

/**
 *   Set Lcd to a specified location given by row and column information
 *   Row Number (1 to 2)
 *   Column Number (1 to 16) Assuming a 2 X 16 characters display
 */
void lcd_set_cursor(uint8_t row, uint8_t column)
{
	column--;
	switch (row)
	{
	case 1:
		/* Set cursor to 1st row address and add index*/
		lcd_send_command((column |= 0x80));
		break;
	case 2:
		/* Set cursor to 2nd row address and add index*/
		lcd_send_command((column |= 0xC0));
		break;
	default:
		break;
	}
}

static void lcd_enable(void)
{
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_SET);
	udelay(5);
	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);
	udelay(50);/* execution time > 37 micro seconds */
}

static void mdelay(uint32_t cnt)
{
	for (uint32_t i = 0; i < (cnt * 1000); i++)
		;
}

static void udelay(uint32_t cnt)
{
	for (uint32_t i = 0; i < (cnt * 1); i++)
		;
}

/*
 * code i2c lcd
 *
 *
 *
 *
 *
 *
 * */

void lcd_send_cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = (uint8_t) data_u | 0x0C;  //en=1, rs=0
	data_t[1] = (uint8_t) data_u | 0x08;  //en=0, rs=0
	data_t[2] = (uint8_t) data_l | 0x0C;  //en=1, rs=0
	data_t[3] = (uint8_t) data_l | 0x08;  //en=0, rs=0

	I2C_MasterSendData(&hi2c2, (uint8_t*) data_t, 4, SLAVE_ADDRESS_LCD, 0);

}

void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = (uint8_t) data_u | 0x0D;  //en=1, rs=0
	data_t[1] = (uint8_t) data_u | 0x09;  //en=0, rs=0
	data_t[2] = (uint8_t) data_l | 0x0D;  //en=1, rs=0
	data_t[3] = (uint8_t) data_l | 0x09;  //en=0, rs=0
	I2C_MasterSendData(&hi2c2, (uint8_t*) data_t, 4, SLAVE_ADDRESS_LCD, 0);
}

void lcd_clear(void)
{
	lcd_send_cmd(0x80);
	for (int i = 0; i < 70; i++)
	{
		lcd_send_data(' ');
	}
}

void lcd_put_cur(int row, int col)
{
	row--;
	col--;
	switch (row)
	{
	case 0:
		lcd_send_cmd((col |= 0x80));
		break;
	case 1:
		lcd_send_cmd((col |= 0xC0));
		break;
	}
	mdelay(2);

}

void lcd_init_i2c(void)
{
	//1. Configure the gpio pins which are used for lcd connections

	GPIO_Handle_t lcd_signal_sda, lcd_signal_scl;

	memset(&lcd_signal_sda, 0, sizeof(lcd_signal_sda));
	memset(&lcd_signal_scl, 0, sizeof(lcd_signal_scl));

	/*
	 * I2C2_SCL ==> PB10
	 * I2C2_SDA ==> PB3
	 */

	lcd_signal_sda.pGPIOx = GPIOB;
	lcd_signal_sda.GPIO_PinConfig.GPIO_PinAltFunMode = 9;
	lcd_signal_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	lcd_signal_sda.GPIO_PinConfig.GPIO_PinNumber = LCD_I2C2_SDA;
	lcd_signal_sda.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	lcd_signal_sda.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	lcd_signal_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&lcd_signal_sda);

	lcd_signal_scl.pGPIOx = GPIOB;
	lcd_signal_scl.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	lcd_signal_scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	lcd_signal_scl.GPIO_PinConfig.GPIO_PinNumber = LCD_I2C2_SCL;
	lcd_signal_scl.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	lcd_signal_scl.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	lcd_signal_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

	GPIO_Init(&lcd_signal_scl);

	hi2c2.pI2Cx = LCD_I2C2;
	hi2c2.I2C_Config.I2C_AckControl = I2C_ACK_ENABLE;
	hi2c2.I2C_Config.I2C_SCLSpeed = LCD_I2C_SPEED;
	I2C_Init(&hi2c2);

	//3. Enable the I2C peripheral
	I2C_PeripheralControl(LCD_I2C2, ENABLE);

	// 4 bit initialisation
	mdelay(50);  // wait for >40ms
	lcd_send_cmd(0x30);
	mdelay(5);  // wait for >4.1ms
	lcd_send_cmd(0x30);
	mdelay(1);  // wait for >100us
	lcd_send_cmd(0x30);
	mdelay(10);
	lcd_send_cmd(0x20);  // 4bit mode
	mdelay(10);

	// dislay initialisation

	//function set command
	lcd_send_cmd(LCD_CMD_4DL_2N_5X8F);

	//disply ON and cursor ON
	lcd_send_cmd(LCD_CMD_DON_CURON);
//	lcd_send_cmd(0x08);

	lcd_send_cmd(LCD_CMD_DIS_CLEAR);

	//entry mode set--> I/D = 1 (increment cursor) & S = 0 (no shift)
	lcd_send_cmd(LCD_CMD_INCADD);

	//Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
//	lcd_send_cmd(0x0C);

}

void lcd_send_string(char *str)
{
	while (*str)
		lcd_send_data(*str++);
}

void lcd_display_return_home_i2c(void)
{

	lcd_send_cmd(LCD_CMD_DIS_RETURN_HOME);
	/*
	 * check page number 24 of datasheet.
	 * return home command execution wait time is around 2ms
	 */
	mdelay(2);
}
