#include <xc.h>

#define _XTAL_FREQ 20000000

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

//----------------------------
// Function Prototypes
//----------------------------
void I2C_Init(void);
void I2C_Wait(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Write(unsigned char data);

//----------------------------
// I2C Initialization
//----------------------------
void I2C_Init(void)
{
    TRISC3 = 1;      // SCL
    TRISC4 = 1;      // SDA

    SSPSTAT = 0x00;
    SSPCON  = 0x28;      // I2C Master Mode
    SSPCON2 = 0x00;
    SSPADD  = 49;        // 100kHz @20MHz
}

//----------------------------
// Wait Until Bus Free
//----------------------------
void I2C_Wait(void)
{
    while ((SSPCON2 & 0x1F) || SSPSTATbits.R_nW);
}

//----------------------------
// START
//----------------------------
void I2C_Start(void)
{
    I2C_Wait();
    SEN = 1;
    while(SEN);
}

//----------------------------
// STOP
//----------------------------
void I2C_Stop(void)
{
    I2C_Wait();
    PEN = 1;
    while(PEN);
}

//----------------------------
// WRITE BYTE
//----------------------------
void I2C_Write(unsigned char data)
{
    I2C_Wait();

    SSPBUF = data;

    while(BF);
    while(SSPSTATbits.R_nW);
}

//----------------------------
// MAIN
//----------------------------
void main(void)
{
    I2C_Init();

    while(1)
    {
        __delay_ms(1000);

        I2C_Start();

        // Send Address
        I2C_Write(0xA0);

        // Send Data
        I2C_Write(0x55);

        I2C_Stop();
    }
}
