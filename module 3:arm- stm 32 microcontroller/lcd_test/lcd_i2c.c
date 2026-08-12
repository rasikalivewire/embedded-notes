/*
 * lcd_i2c.c
 *
 *
 */

#include "lcd_i2c.h"

#define LCD_ADDR (0x27<<1)

#define RS 0x01
#define EN 0x04
#define BL 0x08

static void LCD_Write(uint8_t data)
{
    HAL_I2C_Master_Transmit(&hi2c1,LCD_ADDR,&data,1,100);
}

static void LCD_Enable(uint8_t data)
{
    LCD_Write(data | EN);
    HAL_Delay(1);

    LCD_Write(data & ~EN);
    HAL_Delay(1);
}

static void LCD_Send4Bits(uint8_t nibble)
{
    LCD_Write(nibble);
    LCD_Enable(nibble);
}

void LCD_Send_Command(uint8_t cmd)
{
    uint8_t high,low;

    high=(cmd & 0xF0)|BL;
    low=((cmd<<4)&0xF0)|BL;

    LCD_Send4Bits(high);
    LCD_Send4Bits(low);

    HAL_Delay(2);
}

void LCD_Send_Data(uint8_t data)
{
    uint8_t high,low;

    high=(data & 0xF0)|RS|BL;
    low=((data<<4)&0xF0)|RS|BL;

    LCD_Send4Bits(high);
    LCD_Send4Bits(low);
}

void LCD_Clear(void)
{
    LCD_Send_Command(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row,uint8_t col)
{
    uint8_t addr;

    if(row==0)
        addr=0x80+col;
    else
        addr=0xC0+col;

    LCD_Send_Command(addr);
}

void LCD_Send_String(char *str)
{
    while(*str)
    {
        LCD_Send_Data(*str++);
    }
}

void LCD_Init(void)
{
    HAL_Delay(50);

    LCD_Send4Bits(0x30|BL);
    HAL_Delay(5);

    LCD_Send4Bits(0x30|BL);
    HAL_Delay(5);

    LCD_Send4Bits(0x30|BL);
    HAL_Delay(5);

    LCD_Send4Bits(0x20|BL);

    LCD_Send_Command(0x28);
    LCD_Send_Command(0x0C);
    LCD_Send_Command(0x06);
    LCD_Send_Command(0x01);

    HAL_Delay(5);
}
