#include <xc.h>

#define _XTAL_FREQ 20000000

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

volatile unsigned int count = 0;

void __interrupt() ISR(void)
{
    if (INTCONbits.TMR0IF)
    {
        TMR0 = 61;
        INTCONbits.TMR0IF = 0;

        count++;

        if (count >= 100)
        {
            count = 0;
            RB1 = !RB1;
        }
    }
}

void main(void)
{
    TRISBbits.TRISB1 = 0;
    RB1 = 0;

    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 0;

    OPTION_REGbits.PS2 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS0 = 1;

    TMR0 = 61;

    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;

    while(1)
    {
    }
}
