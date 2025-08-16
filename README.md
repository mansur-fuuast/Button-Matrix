# 2 X 2 Button Matrix with PIC16F627A Microcontroller
Its a MPLab Project for 2x2 Button Matrix and PIC16F627A microcontroller integration to switch corresponding LEDs ON/OFF. You can customize it to control anything you want. It is generally written to describe how can you integrate button matrix with  PIC Microcontrollers.

# Description
Four SPST Buttons are connected to PIC in 2 X 2 Matrix (02 Coulmns and 02 Rows). Pins RA2 and RA3 of PIC microcontroller representing coulmns of matrix are initially set to logic level LOW (0VDC) programmatically. While pins RA4 and RB0 representing rows of matrix are set to logic level HIGH (5VDC) initially.

The Matrix coulmns are switched between input / output pins and then listened for corresponding Row for logic level LOW, whichever pin is pulled to LOW corresponding switch press is detected. 
