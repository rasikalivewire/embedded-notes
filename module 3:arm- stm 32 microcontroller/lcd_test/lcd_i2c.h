/*
 * lcd_i2c.h
 *
 *  Created on: Jul 16, 2026
 *     
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f1xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

void LCD_Init(void);
void LCD_Clear(void);

void LCD_Send_Command(uint8_t cmd);
void LCD_Send_Data(uint8_t data);

void LCD_SetCursor(uint8_t row,uint8_t col);

void LCD_Send_String(char *str);

#endif
