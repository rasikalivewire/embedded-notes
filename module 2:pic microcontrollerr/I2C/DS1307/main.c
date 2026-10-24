
#include <xc.h>
#include <stdint.h>
#include <stdio.h>

#define _XTAL_FREQ 20000000UL

// PIC16F877A configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define RTC_WRITE 0xD0
#define RTC_READ  0xD1

// -------- I2C INITIALIZATION --------
void I2C_Init(void)
{
    TRISC3 = 1;
    TRISC4 = 1;

    SSPCON = 0x28;       // I2C Master mode
    SSPCON2 = 0x00;
    SSPSTAT = 0x80;
    SSPADD = 49;         // 100 kHz at 20 MHz
}

void I2C_Wait(void)
{
    while ((SSPCON2 & 0x1F) || (SSPSTAT & 0x04));
}

void I2C_Start(void)
{
    I2C_Wait();
    SEN = 1;
    while (SEN);
}

void I2C_Restart(void)
{
    I2C_Wait();
    RSEN = 1;
    while (RSEN);
}

void I2C_Stop(void)
{
    I2C_Wait();
    PEN = 1;
    while (PEN);
}

void I2C_Write(uint8_t data)
{
    I2C_Wait();
    SSPBUF = data;
    while (BF);
    I2C_Wait();
}

uint8_t I2C_Read(uint8_t send_ack)
{
    uint8_t data;

    I2C_Wait();
    RCEN = 1;
    while (!BF);

    data = SSPBUF;

    I2C_Wait();
    ACKDT = send_ack ? 0 : 1;
    ACKEN = 1;
    while (ACKEN);

    return data;
}

// -------- UART INITIALIZATION --------
void UART_Init(void)
{
    TRISC6 = 0;    // TX output
    TRISC7 = 1;    // RX input

    TXSTA = 0x24;  // Asynchronous, BRGH=1, TX enabled
    RCSTA = 0x80;  // Serial port enabled
    SPBRG = 129;   // Approximately 9600 baud at 20 MHz
}

void UART_Tx(char c)
{
    while (!TXIF);
    TXREG = c;
}

void UART_String(const char *s)
{
    while (*s)
        UART_Tx(*s++);
}

// -------- RTC FUNCTIONS --------
uint8_t BCD_To_Dec(uint8_t x)
{
    return ((x >> 4) * 10) + (x & 0x0F);
}

uint8_t Dec_To_BCD(uint8_t x)
{
    return ((x / 10) << 4) | (x % 10);
}

// Example initial time: 10:30:00, 24 October 2026
void RTC_SetTime(void)
{
    I2C_Start();
    I2C_Write(RTC_WRITE);
    I2C_Write(0x00);

    I2C_Write(0x00); // Seconds, CH=0
    I2C_Write(0x30); // Minutes
    I2C_Write(0x10); // Hours, 24-hour mode
    I2C_Write(0x07); // Day of week
    I2C_Write(0x24); // Date
    I2C_Write(0x10); // Month
    I2C_Write(0x26); // Year: 2026

    I2C_Stop();
}

void RTC_ReadTime(uint8_t *t)
{
    uint8_t i;

    I2C_Start();
    I2C_Write(RTC_WRITE);
    I2C_Write(0x00);

    I2C_Restart();
    I2C_Write(RTC_READ);

    for (i = 0; i < 6; i++)
        t[i] = I2C_Read(1);  // ACK

    t[6] = I2C_Read(0);      // NACK last byte
    I2C_Stop();

    t[0] = BCD_To_Dec(t[0] & 0x7F);
    t[1] = BCD_To_Dec(t[1] & 0x7F);
    t[2] = BCD_To_Dec(t[2] & 0x3F);
    t[3] = BCD_To_Dec(t[3] & 0x07);
    t[4] = BCD_To_Dec(t[4] & 0x3F);
    t[5] = BCD_To_Dec(t[5] & 0x1F);
    t[6] = BCD_To_Dec(t[6]);
}

// -------- MAIN PROGRAM --------
void main(void)
{
    uint8_t t[7];
    char msg[40];

    ADCON1 = 0x06;  // Digital I/O configuration
    I2C_Init();
    UART_Init();

    __delay_ms(100);

    UART_String("\r\nDS1307 RTC Started\r\n");

    // Set the sample time only during initial testing.
    // Comment this line after setting the RTC once.
    RTC_SetTime();

    while (1)
    {
        RTC_ReadTime(t);

        sprintf(msg, "Time: %02u:%02u:%02u\r\n",
                (unsigned int)t[2],
                (unsigned int)t[1],
                (unsigned int)t[0]);
        UART_String(msg);

        sprintf(msg, "Date: %02u/%02u/20%02u\r\n\r\n",
                (unsigned int)t[4],
                (unsigned int)t[5],
                (unsigned int)t[6]);
        UART_String(msg);

        __delay_ms(1000);
    }
}
