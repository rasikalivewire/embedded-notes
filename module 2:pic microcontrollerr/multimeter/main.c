#include <xc.h>
#include <stdio.h>

// Configuration Bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000 

// LCD Pin Definitions
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

// Buzzer Pin
#define BUZZER RC0

// --- LCD Driver Functions ---
void Lcd_Port(char a) {
    if(a & 1) D4 = 1; else D4 = 0;
    if(a & 2) D5 = 1; else D5 = 0;
    if(a & 4) D6 = 1; else D6 = 0;
    if(a & 8) D7 = 1; else D7 = 0;
}

void Lcd_Cmd(char a) {
    RS = 0;             
    Lcd_Port(a >> 4);   
    EN = 1; __delay_ms(1); EN = 0;
    Lcd_Port(a);        
    EN = 1; __delay_ms(1); EN = 0;
}

void Lcd_Write_Char(char a) {
    RS = 1;             
    Lcd_Port(a >> 4);
    EN = 1; __delay_ms(1); EN = 0;
    Lcd_Port(a);
    EN = 1; __delay_ms(1); EN = 0;
}

void Lcd_Write_String(char *a) {
    for(int i=0; a[i]!='\0'; i++) {
        Lcd_Write_Char(a[i]);
    }
}

void Lcd_Set_Cursor(char row, char col) {
    char temp;
    if(row == 1) {
        temp = 0x80 + col - 1;
        Lcd_Cmd(temp);
    } else if(row == 2) {
        temp = 0xC0 + col - 1;
        Lcd_Cmd(temp);
    }
}

void Lcd_Init() {
    TRISD = 0x00;       
    Lcd_Port(0x00);
    __delay_ms(20);
    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);
    Lcd_Cmd(0x02);      
    Lcd_Cmd(0x28);      
    Lcd_Cmd(0x0C);      
    Lcd_Cmd(0x06);      
    Lcd_Cmd(0x01);      
}

// --- ADC Functions ---
void ADC_Init() {
    ADCON1 = 0x80;      
    ADCON0 = 0x41;      
}

unsigned int ADC_Read(unsigned char channel) {
    ADCON0 &= 0xC5;             
    ADCON0 |= (channel << 3);   
    __delay_ms(2);              
    GO_nDONE = 1;               
    while(GO_nDONE);            
    return ((ADRESH << 8) + ADRESL); 
}

void main() {
    unsigned int adc_value;
    float voltage, current, resistance, v_adc;
    char disp_str[16];
    char current_mode = 0;
    char last_mode = 99; // Force initial screen update

    // I/O Configuration
    TRISA = 0xFF;       // PORTA as inputs (ADC)
    TRISC0 = 0;         // RC0 as output (Buzzer)
    BUZZER = 0;         
    
    // Switch Configuration
    TRISB |= 0x07;           // Set RB0, RB1, RB2 as inputs
    OPTION_REGbits.nRBPU = 0; // Enable internal pull-ups on PORTB
    
    ADC_Init();
    Lcd_Init();
    
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("Starting DMM...");
    __delay_ms(1000);

    while(1) {
        // --- 1. Read the Switch Mode ---
        // Since common is grounded, active pin is LOW (0)
        if(PORTBbits.RB0 == 0) {
            current_mode = 1; // Voltmeter
        } else if(PORTBbits.RB1 == 0) {
            current_mode = 2; // Ammeter
        } else if(PORTBbits.RB2 == 0) {
            current_mode = 3; // Ohmmeter/Continuity
        } else {
            current_mode = 0; // No selection or switch bouncing
        }

        // --- 2. Update LCD UI if mode changed ---
        if(current_mode != last_mode) {
            Lcd_Cmd(0x01); // Clear screen
            Lcd_Set_Cursor(1, 1);
            if(current_mode == 1) Lcd_Write_String("MODE: VOLTMETER");
            if(current_mode == 2) Lcd_Write_String("MODE: AMMETER");
            if(current_mode == 3) Lcd_Write_String("MODE: OHMMETER");
            if(current_mode == 0) Lcd_Write_String("SELECT A MODE");
            BUZZER = 0; // Ensure buzzer turns off when switching modes
            last_mode = current_mode;
        }

        // --- 3. Measure and Display based on Mode ---
        if (current_mode == 1) {
            adc_value = ADC_Read(0);
            v_adc = (adc_value * 5.0) / 1023.0;
            voltage = v_adc * 11.0; 
            
            sprintf(disp_str, "DC: %5.2f V    ", voltage);
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String(disp_str);
        }
        
        else if (current_mode == 2) {
            adc_value = ADC_Read(1);
            v_adc = (adc_value * 5.0) / 1023.0;
            current = v_adc / 0.1;
            
            sprintf(disp_str, "DC: %5.4f A    ", current);
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String(disp_str);
        }
        
else if (current_mode == 3) {
            adc_value = ADC_Read(2);
            v_adc = (adc_value * 5.0) / 1023.0;
            
            // CORRECTED LOGIC: If voltage is near 0, the circuit is open
            if (v_adc < 0.05) {
                resistance = 999999; // Open circuit representation
            } else {
                resistance = 10000.0 * ((5.0 - v_adc) / v_adc); 
            }

            // Continuity Check (Triggers if under 50 Ohms)
            if(resistance < 50.0) {
                BUZZER = 1;
                Lcd_Set_Cursor(2, 1);
                Lcd_Write_String("SHORT (CONT!)   ");
            } else {
                BUZZER = 0;
                Lcd_Set_Cursor(2, 1);
                if(resistance > 99999) {
                    Lcd_Write_String("OPEN CIRCUIT    ");
                } else if(resistance > 999) {
                    sprintf(disp_str, "R: %5.1f kOhm ", resistance/1000.0);
                    Lcd_Write_String(disp_str);
                } else {
                    sprintf(disp_str, "R: %5.0f Ohm  ", resistance);
                    Lcd_Write_String(disp_str);
                }
            }
        }

        __delay_ms(200); // 5Hz refresh rate for smooth UI
    }
}
