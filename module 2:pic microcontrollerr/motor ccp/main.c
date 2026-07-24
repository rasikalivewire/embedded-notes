/*
 * File:   main.c
 * Author: Surya
 *
 * Created on 2 July, 2026, 1:07 PM
 */

#include <xc.h>

#define _XTAL_FREQ 20000000

#pragma config FOSC=HS
#pragma config WDTE=OFF
#pragma config PWRTE=ON
#pragma config BOREN=ON
#pragma config LVP=OFF
#pragma config CPD=OFF
#pragma config WRT=OFF
#pragma config CP=OFF

void ADC_Init()
{
    ADCON0 = 0x41;
    ADCON1 = 0x80;
}

unsigned int ADC_Read()
{
    GO_nDONE = 1;
    while(GO_nDONE);

    return ((ADRESH<<8)+ADRESL);
}

void PWM_Init()
{
    TRISC2 = 0;

    PR2 = 255;

    CCP1CON = 0b00001100;

    T2CON = 0b00000111;
}

void PWM_SetDuty(unsigned int duty)
{
    if(duty > 1023)
        duty = 1023;

    CCPR1L = duty >> 2;

    CCP1CON &= 0xCF;
    CCP1CON |= (duty & 0x03)<<4;
}

void main()
{
    ADC_Init();

    PWM_Init();

    TRISD = 0x00;

    RD0 = 1;
    RD1 = 0;

    unsigned int adc;

    while(1)
    {
        adc = ADC_Read();

        PWM_SetDuty(adc);

        __delay_ms(20);
    }
}
