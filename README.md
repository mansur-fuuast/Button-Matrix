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

// Configuration bits
#pragma config FOSC = INTOSCIO, WDTE = OFF, PWRTE = OFF, MCLRE = OFF
#pragma config BOREN = OFF, LVP = OFF, CPD = OFF, CP = ON

// LEDs (active LOW: anodes to +5V via resistors, cathodes to RB1..RB4)
#define LED_AUTO PORTBbits.RB1
#define LED_FULL PORTBbits.RB2
#define LED_STEP PORTBbits.RB3
#define LED_STOP PORTBbits.RB4

//All LEDs OFF function
static void leds_off(void) {
    LED_AUTO = LED_FULL = LED_STEP = LED_STOP = 1;    //When negative terminal is active HIGH LEDs are OFF
}

// return 1..4 (AUTO,FULL,STEP,STOP) or 0 if none
static unsigned char scan_matrix(void) {
    unsigned char pressed_button = 0;

    // Rows are inputs (read them); external 10k pull-ups hold them HIGH
    TRISBbits.TRISB0 = 1;     // RB0 input
    TRISAbits.TRISA4 = 1;     // RA4 input

    // --- Drive Column RA2 LOW, RA3 Hi-Z ---
    TRISAbits.TRISA2 = 0;    //RA2 output
    PORTAbits.RA2 = 0;       //Set RA2 LOW (active column)
    TRISAbits.TRISA3 = 1;    //RA3 input (Hi-Z (impedance))
    __delay_us(50);          //settle time for CPU

    if (PORTBbits.RB0 == 0) pressed_button = 1;            // Button 1 pressed (RB0-RA2)
    if (PORTAbits.RA4 == 0) pressed_button = 2;            // Button 2 pressed (RA4-RA2)

    // Release RA2
    TRISAbits.TRISA2 = 1;                      (Hi-Z (High impedance))

    // --- Drive Column RA3 LOW, RA2 Hi-Z ---
    TRISAbits.TRISA3 = 0;    // RA3 output
    PORTAbits.RA3 = 0;       // RA3 set to LOW (active column)
    TRISAbits.TRISA2 = 1;    // RA2 input (Hi-Z)
    __delay_us(50);          //settle time for CPU

    if (PORTBbits.RB0 == 0) pressed_button = 3;            // Button 3 pressed (RB0-RA3)
    if (PORTAbits.RA4 == 0) pressed_button = 4;            // Button 4 pressed (RA4-RA3)

    // Release RA3
    TRISAbits.TRISA3 = 1;

    return pressed_button;                                  // 0 if none detected
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
    TRISBbits.TRISB0 = 1;  // RB0 input (10k to VDD)
    TRISAbits.TRISA4 = 1;  // RA4 input (10k to VDD)

    // Columns start as inputs (Hi-Z)
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;

    unsigned char latched = 0;

    while (1) {
        unsigned char pressed_button = scan_matrix();

        if (pressed_button != 0 && pressed_button != latched) {
            latched = pressed_button;
            leds_off();
            if (pressed_button == 1) LED_AUTO = 0;  // AUTO
            if (pressed_button == 2) LED_FULL = 0;  // FULL
            if (pressed_button == 3) LED_STEP = 0;  // STEP
            if (pressed_button == 4) LED_STOP = 0;  // STOP (or turn all off if you prefer)
        }

        // Simple debounce / CPU relax
        __delay_ms(10);
    }
}
```
# Usage
1. Create a MPLab new project. 

2. Select **"Microchip Embedded"** in Categories tab and **"Application Project(s)"** in Projects tab. Click Next
<img width="958" height="503" alt="MPlab 1" src="https://github.com/user-attachments/assets/8a11c190-6699-41ad-b95f-8f5aa16ced1a" />

3. In next screen,
   (i)    Select **"Mid-Range 8-bit MCUs (PIC10/12/16/MCP)"** from Family dropdown,
   (ii)   Select **"PIC16F627A"** from Device dropdown, and
   (iii)  Select **"Simulator"** from Tool dropdown and click next.
<img width="959" height="504" alt="MPlab 2" src="https://github.com/user-attachments/assets/cafb3a7e-1889-40c3-9800-122e185480f2" />

4. In next screen select **"Supported Debug Header"** from the drop down if you want to debug your app. Click Next
<img width="959" height="502" alt="MPlab 3" src="https://github.com/user-attachments/assets/e42994af-cb17-4e04-ad65-2d3e58c650e8" />

5. In the Select Compiler Window, Select XC8 from the window, Click Next.
<img width="959" height="503" alt="MPlab 4" src="https://github.com/user-attachments/assets/43b2935b-0330-4f23-a411-99519c2072fc" />

6. Enter your project name, and click finish
   <img width="959" height="503" alt="MPlab 5" src="https://github.com/user-attachments/assets/7803c04a-2115-4cfb-8bb8-9f80e5d621be" />

7. Your project will be created but now it has no files, add file by clicking the add file button. A Window will open like given here. Select and Expand **"Microchip Embedded"** in the categories tab and further select "XC8 Compiler". In the File types select **"main.c"**. Click Next
<img width="959" height="503" alt="MPlab 6" src="https://github.com/user-attachments/assets/a7a67df1-fbf0-4292-b23d-bb50597263eb" />
<img width="959" height="503" alt="MPlab 7" src="https://github.com/user-attachments/assets/a2a3ec34-1f00-4040-ad74-75f9fca43249" />

8. In next screen, change name of file if you want or leave it as main.c, Click finish. Your c code file has been generated with minimal code.
   <img width="959" height="502" alt="MPlab 9" src="https://github.com/user-attachments/assets/f81ca523-f8d2-40d6-acab-392f6e6ed0a6" />

9. Download main.c from this reposiry or copy paste the complete code given in this readme.md file into your now created file in MPLab IDE.

10. Press F11, or in the main menu select **Production** and click **Build Main Project**

11. Wait for the compiler to build your project and verify in Output Window Project Build Successfully appears in greaan font.
<img width="959" height="503" alt="image" src="https://github.com/user-attachments/assets/04a9558b-1a26-40ed-a39d-d3be4ce3e998" />

12. You can create a scematic in the Proteus IDE like the given below:

13. Double click the PIC16F627A microcontroller and a window will appear like below:
<img width="959" height="504" alt="image" src="https://github.com/user-attachments/assets/1bb594d6-a4dd-4573-9314-19bbc834ec29" />

15. Click on the folder icon in front of **Program File** option, navigate to your project folder ```-> dist -> default -> production``` and select the .hex file.

16. Click OK, and while getting back to your project scematics in Proteus click Run Simulation. Hurrah! if you have output like this, you programmed the micrcontroller successfully!
