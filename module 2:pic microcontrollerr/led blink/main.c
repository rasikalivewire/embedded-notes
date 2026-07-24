
#include <xc.h>


// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 20000000

void main() 
{
    TRISB1 = 0; //tri state buffer(i/p or o/p)
//    TRISB2 = 0;
//    TRISB3 = 0;
//    TRISB4 = 0;
//    TRISB5 = 0;
//    TRISB6 = 0;
    PORTBbits.RB1 = 0;  //state of the pin(high or low)
//    PORTBbits.RB2 = 0;
//    PORTBbits.RB3 = 0;
//    PORTBbits.RB4 = 0;
//    PORTBbits.RB5 = 0;
//    PORTBbits.RB6 = 0;

    while(1)
    {

        RB1 = !RB1;
        __delay_ms(1000);
    }
    return;
}
