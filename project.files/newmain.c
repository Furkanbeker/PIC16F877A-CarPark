// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (single-supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"

#define _XTAL_FREQ 8000000

#define TRIG RB0
#define ECHO RB1
#define LED1 RC0
#define LED2 RC1
#define LED3 RC2
#define LED4 RC3
#define BUZZER RC4

void Trigger_Ultrasonic();
int Get_Distance();
void Update_LEDs(int distance);
void Update_Buzzer(int distance);

int main() {
    TRISB = 0x02;  // RB0: Output (TRIG), RB1: Input (ECHO)
    TRISC = 0x00;  // RC0-RC4: Output (LEDs ve Buzzer)
    TRISD = 0x00;  // RD0-RD6: Output (LCD)
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    Lcd_Init();
    
    int distance;
    char distanceStr[16];
    
    while (1) {
        Trigger_Ultrasonic();
        distance = Get_Distance();
        
        // Mesafeyi LCD'ye yazd?rma
        Lcd_Clear();
        Lcd_Set_Cursor(1, 1);
        sprintf(distanceStr, "Distance: %d cm", distance);
        Lcd_Write_String(distanceStr);
        
        // LED ve Buzzer kontrol�
        Update_LEDs(distance);
        Update_Buzzer(distance);
        
        __delay_ms(100);  // Bekleme s�resi
    }
}

void Trigger_Ultrasonic() {
    TRIG = 1;
    __delay_us(10);
    TRIG = 0;
}

int Get_Distance() {
    int timeElapsed;
    int distance;

    TMR1 = 0;  // Timer'? s?f?rla
    T1CON = 0x10;  // Timer1 ayarlar?
    while (!ECHO);  // ECHO pininin HIGH olmas?n? bekle
    T1CON |= 0x01;  // Timer1'i ba?lat
    while (ECHO);  // ECHO pininin LOW olmas?n? bekle
    T1CON &= ~0x01;  // Timer1'i durdur
    timeElapsed = TMR1;  // Timer1 de?erini al
    distance = timeElapsed / 58.82;  // Mesafeyi hesapla (cm)
    return distance;
}

void Update_LEDs(int distance) {
    // T�m LED'leri s?f?rla
    LED1 = 0;
    LED2 = 0;
    LED3 = 0;
    LED4 = 0;
    
    if (distance < 5) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
    } else if (distance < 12) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 0;
    } else if (distance < 18) {
        LED1 = 1;
        LED2 = 1;
        LED3 = 0;
        LED4 = 0;
    } else if (distance < 25) {
        LED1 = 1;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
    } else {
        // T�m LED'ler zaten kapal?
    }
}

void Update_Buzzer(int distance) {
    int i;
    if (distance < 5) {
        for (i = 0; i < 100; i++) {  // Daha y�ksek frekans
            BUZZER = 1;
            __delay_us(500);
            BUZZER = 0;
            __delay_us(500);
        }
    } else if (distance < 12) {
        for (i = 0; i < 50; i++) {  // Orta frekans
            BUZZER = 1;
            __delay_ms(2);
            BUZZER = 0;
            __delay_ms(2);
        }
    } else if (distance < 18) {
        for (i = 0; i < 25; i++) {  // Daha d�?�k frekans
            BUZZER = 1;
            __delay_ms(5);
            BUZZER = 0;
            __delay_ms(5);
        }
    } else if (distance < 25) {
        for (i = 0; i < 10; i++) {  // En d�?�k frekans
            BUZZER = 1;
            __delay_ms(10);
            BUZZER = 0;
            __delay_ms(10);
        }
    } else {
        BUZZER = 0;  // Buzzer kapal?
    }
}
