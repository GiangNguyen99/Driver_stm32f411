################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/Src/15_I2C_lcd1602_rtc.c \
../drivers/Src/ds3231.c \
../drivers/Src/lcd.c \
../drivers/Src/stm32f411xx.spi_driver.c \
../drivers/Src/stm32f411xx_gpio_driver.c \
../drivers/Src/stm32f411xx_i2c_driver.c \
../drivers/Src/stm32f411xx_rcc_driver.c \
../drivers/Src/stm32f411xx_usart_driver.c 

OBJS += \
./drivers/Src/15_I2C_lcd1602_rtc.o \
./drivers/Src/ds3231.o \
./drivers/Src/lcd.o \
./drivers/Src/stm32f411xx.spi_driver.o \
./drivers/Src/stm32f411xx_gpio_driver.o \
./drivers/Src/stm32f411xx_i2c_driver.o \
./drivers/Src/stm32f411xx_rcc_driver.o \
./drivers/Src/stm32f411xx_usart_driver.o 

C_DEPS += \
./drivers/Src/15_I2C_lcd1602_rtc.d \
./drivers/Src/ds3231.d \
./drivers/Src/lcd.d \
./drivers/Src/stm32f411xx.spi_driver.d \
./drivers/Src/stm32f411xx_gpio_driver.d \
./drivers/Src/stm32f411xx_i2c_driver.d \
./drivers/Src/stm32f411xx_rcc_driver.d \
./drivers/Src/stm32f411xx_usart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/Src/%.o: ../drivers/Src/%.c drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F411VETx -DSTM32 -DSTM32F4 -DSTM32F411E_DISCO -c -I../Inc -I"C:/Users/hoang/OneDrive - ut.edu.vn/Desktop/PROJECT/my code/PROJECT MASTERING MCU/stm32_f4xx.drivers/drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-drivers-2f-Src

clean-drivers-2f-Src:
	-$(RM) ./drivers/Src/15_I2C_lcd1602_rtc.d ./drivers/Src/15_I2C_lcd1602_rtc.o ./drivers/Src/ds3231.d ./drivers/Src/ds3231.o ./drivers/Src/lcd.d ./drivers/Src/lcd.o ./drivers/Src/stm32f411xx.spi_driver.d ./drivers/Src/stm32f411xx.spi_driver.o ./drivers/Src/stm32f411xx_gpio_driver.d ./drivers/Src/stm32f411xx_gpio_driver.o ./drivers/Src/stm32f411xx_i2c_driver.d ./drivers/Src/stm32f411xx_i2c_driver.o ./drivers/Src/stm32f411xx_rcc_driver.d ./drivers/Src/stm32f411xx_rcc_driver.o ./drivers/Src/stm32f411xx_usart_driver.d ./drivers/Src/stm32f411xx_usart_driver.o

.PHONY: clean-drivers-2f-Src

