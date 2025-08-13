/*
 2x2 Button Matrix to control 04 LEDs
 * Designed for PIC16F627A Microcontroller ship
 * 
 * Author: M Mansoor Ahmed
 * Email: m.mansoor.ahmed@outlook.com
 * Whatsapp: +92-313-123-3939
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

    if (PORTBbits.RB0 == 0) sw = 1;            // AUTO (RB0–RA2)
    if (PORTAbits.RA4 == 0) sw = 2;            // FULL (RA4–RA2)

    // Release RA2
    TRISAbits.TRISA2 = 1;                      // Hi-Z

    // --- Drive Column RA3 LOW, RA2 Hi-Z ---
    TRISAbits.TRISA3 = 0; PORTAbits.RA3 = 0;   // RA3 OUT LOW (active column)
    TRISAbits.TRISA2 = 1;                      // RA2 IN (Hi-Z)
    __delay_us(50);

    if (PORTBbits.RB0 == 0) sw = 3;            // STEP (RB0–RA3)
    if (PORTAbits.RA4 == 0) sw = 4;            // STOP (RA4–RA3)

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
