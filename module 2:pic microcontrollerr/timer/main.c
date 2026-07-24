/*
 * File:   main.c
 * Author: Surya
 *
 * Created on 23 June, 2026, 10:18 AM
 */

#include <xc.h>

#define _XTAL_FREQ 16000000

// CONFIG
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

void __interrupt() ISR(void)
{
    if(TMR0IF)
    {
        TMR0IF = 0;      // Clear interrupt flag
        TMR0 = 61;     // Reload value

        RB0 = !RB0;    // Toggle LED
    }
}

void main()
{
    TRISB0 = 0;
    RB0 = 0;

    OPTION_REG = 0x07;
    // Prescaler = 256
    // Assigned to Timer0

    TMR0 = 61;

    INTCONbits.T0IE = 1;     // Enable Timer0 interrupt
    INTCONbits.GIE = 1;      // Global interrupt enable
    INTCONbits.PEIE = 1;
    

    while(1)
    {
    }
}
