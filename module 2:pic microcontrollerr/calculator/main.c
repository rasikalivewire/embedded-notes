#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 20000000

#pragma config FOSC=HS
#pragma config WDTE=OFF
#pragma config PWRTE=ON
#pragma config BOREN=OFF
#pragma config LVP=OFF
#pragma config CPD=OFF
#pragma config WRT=OFF
#pragma config CP=OFF

#define RS RD0
#define EN RD1

char keypad[4][4]={
{'7','8','9','/'},
{'4','5','6','*'},
{'1','2','3','-'},
{'C','0','=','+'}
};

void lcd_cmd(unsigned char);
void lcd_data(unsigned char);
void lcd_init();
void lcd_string(char *);
void lcd_clear();
char keypad_read();

void lcd_cmd(unsigned char cmd)
{
    RS=0;

    PORTD=(PORTD&0x0F)|(cmd&0xF0);
    EN=1;
    __delay_ms(2);
    EN=0;

    PORTD=(PORTD&0x0F)|((cmd<<4)&0xF0);
    EN=1;
    __delay_ms(2);
    EN=0;
}

void lcd_data(unsigned char data)
{
    RS=1;

    PORTD=(PORTD&0x0F)|(data&0xF0);
    EN=1;
    __delay_ms(2);
    EN=0;

    PORTD=(PORTD&0x0F)|((data<<4)&0xF0);
    EN=1;
    __delay_ms(2);
    EN=0;
}

void lcd_init()
{
    __delay_ms(20);

    lcd_cmd(0x02);
    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

void lcd_clear()
{
    lcd_cmd(0x01);
}

void lcd_string(char *s)
{
    while(*s)
        lcd_data(*s++);
}

char keypad_read()
{
    char row,col;

    while(1)
    {
        for(row=0;row<4;row++)
        {
            PORTB=0xFF;

            PORTB &= ~(1<<row);

            for(col=0;col<4;col++)
            {
                if(!(PORTB&(1<<(col+4))))
                {
                    __delay_ms(20);

                    while(!(PORTB&(1<<(col+4))));

                    return keypad[row][col];
                }
            }
        }
    }
}

void main()
{
    TRISD=0x00;

    TRISB=0xF0;

    OPTION_REG=0;

    unsigned long n1=0,n2=0;
    unsigned long ans=0;

    char op=0;
    char k;

    char buf[17];

    lcd_init();

    while(1)
    {
        lcd_clear();
        lcd_string("Calculator");

        __delay_ms(1000);

        lcd_clear();

        n1=0;
        n2=0;
        ans=0;
        op=0;

        while(1)
        {
            k=keypad_read();

            if(k>='0' && k<='9')
            {
                n1=n1*10+(k-'0');
                lcd_data(k);
            }
            else
            {
                op=k;
                lcd_data(k);
                break;
            }
        }

        while(1)
        {
            k=keypad_read();

            if(k>='0' && k<='9')
            {
                n2=n2*10+(k-'0');
                lcd_data(k);
            }
            else if(k=='=')
            {
                break;
            }
        }

        switch(op)
        {
            case '+':
                ans=n1+n2;
                break;

            case '-':
                ans=n1-n2;
                break;

            case '*':
                ans=n1*n2;
                break;

            case '/':
                if(n2!=0)
                    ans=n1/n2;
                else
                    ans=0;
                break;
        }

        lcd_clear();

        sprintf(buf,"%lu",ans);

        lcd_string("Answer:");

        lcd_cmd(0xC0);

        lcd_string(buf);

        while(keypad_read()!='C');
    }
}
