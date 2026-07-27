#include <xc.h>

#define _XTAL_FREQ 20000000

#pragma config FOSC=HS
#pragma config WDTE=OFF
#pragma config PWRTE=ON
#pragma config BOREN=OFF
#pragma config LVP=OFF
#pragma config CPD=OFF
#pragma config WRT=OFF
#pragma config CP=OFF

#define RS PORTDbits.RD0
#define EN PORTDbits.RD1

void lcd_enable(void);
void lcd_send_nibble(unsigned char nibble);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init(void);
void lcd_string(const char *str);

void lcd_enable()
{
    EN = 1;
    __delay_us(2);
    EN = 0;
    __delay_us(100);
}

void lcd_send_nibble(unsigned char nibble)
{
    PORTD &= 0x0F;              // Clear RD4-RD7
    PORTD |= (nibble << 4);     // Put nibble on RD4-RD7
    lcd_enable();
}

void lcd_cmd(unsigned char cmd)
{
    RS = 0;

    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);

    __delay_ms(2);
}

void lcd_data(unsigned char data)
{
    RS = 1;

    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);

    __delay_ms(2);
}

void lcd_init()
{
    TRISD = 0x00;
    PORTD = 0x00;

    __delay_ms(20);

    RS = 0;

    lcd_send_nibble(0x03);
    __delay_ms(5);

    lcd_send_nibble(0x03);
    __delay_us(200);

    lcd_send_nibble(0x03);
    __delay_us(200);

    lcd_send_nibble(0x02);

    lcd_cmd(0x28);    // 4-bit, 2-line
    lcd_cmd(0x0C);    // Display ON
    lcd_cmd(0x06);    // Cursor Increment
    lcd_cmd(0x01);    // Clear Display
}

void lcd_string(const char *str)
{
    while(*str)
        lcd_data(*str++);
}

void main()
{
    lcd_init();

    while(1)
    {
        lcd_cmd(0x80);
        lcd_string("Hello Rasika");

        lcd_cmd(0xC0);
        lcd_string("Team:livewire");
    }
}
