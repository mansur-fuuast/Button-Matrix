# 2 X 2 Button Matrix with PIC16F627A Microcontroller
Its a MPLab Project for 2x2 Button Matrix and PIC16F627A microcontroller integration to switch corresponding LEDs ON/OFF. You can customize it to control anything you want. It is generally written to describe how can you integrate button matrix with  PIC Microcontrollers.

# Description
Four SPST Buttons are connected to PIC in 2 X 2 Matrix (02 Coulmns and 02 Rows). Pins RA2 and RA3 of PIC microcontroller representing coulmns of matrix are initially set to logic level LOW (0VDC) programmatically. While pins RA4 and RB0 representing rows of matrix are set to logic level HIGH (5VDC) initially.
scanning of buttons matrix is performed in below function:
```C Code
static unsigned char scan_button_matrix(void) {
    unsigned char pressed_button = 0;

    // Rows are inputs (read them); external 10k pull-ups hold them HIGH
    TRISBbits.TRISB0 = 1;     // RB0 input
    TRISAbits.TRISA4 = 1;     // RA4 input

    // --- Drive Column RA2 LOW, RA3 Hi-Z ---
    TRISAbits.TRISA2 = 0;
    PORTAbits.RA2 = 0;        // RA2 OUT LOW (active column)
    TRISAbits.TRISA3 = 1;     // RA3 IN (Hi-Z)
    __delay_us(50);           // settle

    if (PORTBbits.RB0 == 0) pressed_button = 1;            // Button 1 is pressed RB0-RA2
    if (PORTAbits.RA4 == 0) pressed_button = 2;            // Button 2 is pressed RA4-RA2

    // Release RA2
    TRISAbits.TRISA2 = 1;                      // Hi-Z (impedance)

    // --- Drive Column RA3 LOW, RA2 Hi-Z ---
    TRISAbits.TRISA3 = 0;
    PORTAbits.RA3 = 0;   // RA3 OUT LOW (active column)
    TRISAbits.TRISA2 = 1;                      // RA2 IN (Hi-Z)
    __delay_us(50);

    if (PORTBbits.RB0 == 0) pressed_button = 3;    // Button 3 is pressed (RB0-RA3)
    if (PORTAbits.RA4 == 0) pressed_button = 4;    // Button 4 is pressed (RA4-RA3)

    // Release RA3
    TRISAbits.TRISA3 = 1;

    return pressed_button;    // 0 if none detected
}
```


The Matrix coulmns are switched between input / output pins and then listened for corresponding Row for logic level LOW, whichever pin is pulled to LOW corresponding switch press is detected.

# Complete Code
```C Code/*
 2x2 Button Matrix to control 04 LEDs
 * Designed for PIC16F627A Microcontroller ship
 * 
 * Author: M Mansoor Ahmed
 * Email: m.mansoor.ahmed@outlook.com
 * 
 * Created on: 13 August, 2025 09:55AM
 */

#define _XTAL_FREQ 4000000
#include <xc.h>

// CONFIG
#pragma config FOSC = INTOSCIO, WDTE = OFF, PWRTE = OFF, MCLRE = OFF
#pragma config BOREN = OFF, LVP = OFF, CPD = OFF, CP = ON

// LEDs (active LOW: anodes to +5V via resistors, cathodes to RB1..RB4)
#define LED_AUTO PORTBbits.RB1
#define LED_FULL PORTBbits.RB2
#define LED_STEP PORTBbits.RB3
#define LED_STOP PORTBbits.RB4

static void leds_off(void) {
    LED_AUTO = LED_FULL = LED_STEP = LED_STOP = 1;
}

// return 1..4 (AUTO,FULL,STEP,STOP) or 0 if none
static unsigned char scan_matrix(void) {
    unsigned char sw = 0;

    // Rows are inputs (read them); external 10k pull-ups hold them HIGH
    TRISBbits.TRISB0 = 1;     // RB0 input
    TRISAbits.TRISA4 = 1;     // RA4 input

    // --- Drive Column RA2 LOW, RA3 Hi-Z ---
    TRISAbits.TRISA2 = 0; PORTAbits.RA2 = 0;  // RA2 OUT LOW (active column)
    TRISAbits.TRISA3 = 1;                      // RA3 IN (Hi-Z)
    __delay_us(50);                            // settle

    if (PORTBbits.RB0 == 0) sw = 1;            // AUTO (RB0?RA2)
    if (PORTAbits.RA4 == 0) sw = 2;            // FULL (RA4?RA2)

    // Release RA2
    TRISAbits.TRISA2 = 1;                      // Hi-Z

    // --- Drive Column RA3 LOW, RA2 Hi-Z ---
    TRISAbits.TRISA3 = 0; PORTAbits.RA3 = 0;   // RA3 OUT LOW (active column)
    TRISAbits.TRISA2 = 1;                      // RA2 IN (Hi-Z)
    __delay_us(50);

    if (PORTBbits.RB0 == 0) sw = 3;            // STEP (RB0?RA3)
    if (PORTAbits.RA4 == 0) sw = 4;            // STOP (RA4?RA3)

    // Release RA3
    TRISAbits.TRISA3 = 1;

    return sw;                                  // 0 if none detected
}

void main(void) {
    CMCON = 0x07;          // all digital

    // LED pins as outputs
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    leds_off();

    // Rows are inputs (with your external pull-ups)
    TRISBbits.TRISB0 = 1;  // RB0 input (10k to VDD on your board)
    TRISAbits.TRISA4 = 1;  // RA4 input (10k to VDD on your board)

    // Columns start as inputs (Hi-Z)
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    unsigned char latched = 0;

    while (1) {
        unsigned char sw = scan_matrix();

        if (sw != 0 && sw != latched) {
            latched = sw;
            leds_off();
            if (sw == 1) LED_AUTO = 0;  // AUTO
            if (sw == 2) LED_FULL = 0;  // FULL
            if (sw == 3) LED_STEP = 0;  // STEP
            if (sw == 4) LED_STOP = 0;  // STOP (or turn all off if you prefer)
        }

        // Simple debounce / CPU relax
        __delay_ms(10);
    }
}
```
# Usage
1. Creat
