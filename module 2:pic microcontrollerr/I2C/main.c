/*
 * File:   main.c
 * Author: Surya
 *
 * Created on 1 July, 2026, 9:11 AM
 */


#include <xc.h>

#define _XTAL_FREQ 20000000


#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//---------------------- I2C Functions ----------------------//
void I2C_Init(void)
{
    TRISC3 = 1;              // SCL
    TRISC4 = 1;              // SDA

    SSPCON = 0x28;           // I2C Master Mode
    SSPCON2 = 0x00;
    SSPSTAT = 0x80;          // Slew rate disabled

    SSPADD = 49;             // 100kHz I2C @20MHz
}

void I2C_Wait(void)
{
    while ((SSPCON2 & 0x1F) || SSPSTATbits.R_W);
}

void I2C_Start(void)
{
    I2C_Wait();
    SSPCON2bits.SEN = 1;
}

void I2C_Stop(void)
{
    I2C_Wait();
    SSPCON2bits.PEN = 1;
}

void I2C_Write(unsigned char data)
{
    I2C_Wait();
    SSPBUF = data;

    while (SSPSTATbits.BF);
    I2C_Wait();
}

//---------------------- Main ----------------------//
void main(void)
{
    I2C_Init();

    while (1)
    {
        I2C_Start();

        I2C_Write(0xA0);      // Slave address (0x50 << 1) + Write

        I2C_Write(0xAA);      // Test Data 1
        I2C_Write(0x55);      // Test Data 2
        I2C_Write(0x12);      // Test Data 3

        I2C_Stop();

        __delay_ms(1000);
    }
}
