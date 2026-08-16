#include <xc.h>

// Configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000

void main(void)
{
    TRISBbits.TRISB0 = 0;   // RB0 as output
    PORTBbits.RB0 = 0;      // LED initially OFF

    while(1)
    {
        PORTBbits.RB0 = 1;  // LED ON
        __delay_ms(500);

        PORTBbits.RB0 = 0;  // LED OFF
        __delay_ms(500);
        
    }
}
