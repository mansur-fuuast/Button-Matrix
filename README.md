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
