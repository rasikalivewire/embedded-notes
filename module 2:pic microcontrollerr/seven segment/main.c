/*
 * File:   main.c
 * Author: Surya
 *
 * Created on 3 July, 2026, 5:18 PM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CC 7-segment codes
const unsigned char s[10] =
{
    0x3F,   //0
    0x06,   //1
    0x5B,   //2
    0x4F,   //3
    0x66,   //4
    0x6D,   //5
    0x7D,   //6
    0x07,   //7
    0x7F,   //8
    0x6F    //9
};

void display4(unsigned char a,
              unsigned char b,
              unsigned char c,
              unsigned char d)
{
    // Digit 1
    PORTD = 0x0E;
    PORTB = s[a];
    __delay_ms(2);

    // Digit 2
    PORTD = 0x0D;
    PORTB = s[b];
    __delay_ms(2);

    // Digit 3
    PORTD = 0x0B;
    PORTB = s[c];
    __delay_ms(2);

    // Digit 4
    PORTD = 0x07;
    PORTB = s[d];
    __delay_ms(2);
}

void main()
{
    TRISB = 0x00;
    TRISD = 0x00;

    PORTB = 0;
    PORTD = 0x0F;

    while(1)
    {
        display4(6,7,8,9);
    }
}
