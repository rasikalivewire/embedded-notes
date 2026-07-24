/*
 * PIC16F877A
 * ADC (AN0) + UART (9600)
 * XC8 Compiler
 */

#include <xc.h>
#include <stdio.h>

/*------------------------------------------------------------------
 Configuration Bits
------------------------------------------------------------------*/
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000UL

/*------------------------------------------------------------------
 Function Prototypes
------------------------------------------------------------------*/
void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(const char *s);

void ADC_Init(void);
unsigned int ADC_Read(unsigned char channel);

/*------------------------------------------------------------------
 UART Initialization
------------------------------------------------------------------*/
void UART_Init(void)
{
    TRISC6 = 1;      // TX pin
    TRISC7 = 1;      // RX pin

    SPBRG = 129;     // 9600 baud @ 20MHz

    BRGH = 1;        // High speed
    SYNC = 0;        // Async mode
    SPEN = 1;        // Enable serial port

    TXEN = 1;        // Enable transmitter
    CREN = 1;        // Enable receiver
}

/*------------------------------------------------------------------
 Send One Character
------------------------------------------------------------------*/
void UART_SendChar(char c)
{
    while(TXIF == 0);

    TXREG = c;
}

/*------------------------------------------------------------------
 Send String
------------------------------------------------------------------*/
void UART_SendString(const char *s)
{
    while(*s)
    {
        UART_SendChar(*s++);
    }
}

/*------------------------------------------------------------------
 ADC Initialization
------------------------------------------------------------------*/
void ADC_Init(void)
{
    TRISA0 = 1;      // AN0 input

    /*
     * ADFM = 1  -> Right Justified
     * AN0 Analog
     * Vref = VDD/VSS
     */
    ADCON1 = 0b10000000;

    /*
     * CHS = 000 -> AN0
     * ADCS = Fosc/32
     * ADON = 1
     */
   // ADCON0 = 0b10000000;
}

/*------------------------------------------------------------------
 ADC Read Function
------------------------------------------------------------------*/
unsigned int ADC_Read(unsigned char channel)
{
    unsigned int result;

    ADCON0 &= 0xC5;                 // Clear channel bits
    ADCON0 |= (channel << 3);       // Select channel

    __delay_us(20);                 // Acquisition time

    GO_DONE = 1;                    // Start conversion

    while(GO_DONE);                 // Wait till complete

    result = ((unsigned int)ADRESH << 8) | ADRESL;

    return result;
}

/*------------------------------------------------------------------
 Main
------------------------------------------------------------------*/
void main(void)
{
    unsigned int adcValue;
    char uartBuffer[30];

    UART_Init();
    ADC_Init();

    UART_SendString("\r\nADC Started\r\n");

    while(1)
    {
        adcValue = ADC_Read(0);

        sprintf(uartBuffer,
                "ADC Value = %u\r\n",
                adcValue);

        UART_SendString(uartBuffer);

        __delay_ms(500);
    }
}
