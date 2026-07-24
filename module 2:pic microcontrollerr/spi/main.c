

#include <xc.h>

#define _XTAL_FREQ 20000000

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// Chip Select (CS) connected to RA5
#define CS PORTCbits.RC6

//---------------- SPI Functions ----------------//

void SPI_Init(void)
{
    TRISC3 = 0;   // SCK Output
    TRISC4 = 1;   // SDI Input
    TRISC5 = 0;   // SDO Output

    TRISC6 = 0;   // CS Output
    CS = 1;                 // Slave deselected

    SSPSTAT = 0x40;         // CKE = 1
    SSPCON  = 0x20;         // SPI Master, Fosc/4
}

unsigned char SPI_Transfer(unsigned char data)
{
    SSPBUF = data;

    while(!SSPSTATbits.BF); // Wait until transmission complete

    return SSPBUF;          // Return received byte
}

//---------------- Main ----------------//

void main(void)
{
    SPI_Init();

    while(1)
    {
        CS = 0;                     // Select slave

        SPI_Transfer(0xAA);         // Test Data 1
        SPI_Transfer(0x55);         // Test Data 2
        SPI_Transfer(0x12);         // Test Data 3

        CS = 1;                     // Deselect slave

        __delay_ms(1000);
    }
}
