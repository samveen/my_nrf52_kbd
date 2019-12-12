<!--
# See LICENSE file for details
-->

# Matrix Design and Considerations

## Overview

The [Bluefruit nRF52 Feather](https://www.adafruit.com/product/3406) has 19 GPIO
pins that I can use for the matrix. Possible indicators needed are power status,
BT status, Caps lock indicator, number lock indicator and charging indicator.

### Indicators

For the indicators, the yellow LED is already the charging indicator, and the
blue LED is tied in with the BT stack. However, the working of the BT stack is
as good an indicator of power as a separate power status indicator. The numlock
isn't in existence here as the number keypad isn't implemented. That leaves the
caps lock and the red LED for it. 

### Scan process

The matrix scan algorithm works as by setting the write lines LOW one by one,
and then checking the read line status. Key presses will have the appropriate
read lines LOW instead of HIGH (no variation from standard scanning).

### Matrix

The number of keys required is 2 sets of keys, each set a matrix of 5 rows and
7 colums, for a total of 70 keys. Given that the number of GPIO pins is 19, the
matrix needs to be such that the number of lines fits within this limit.

With that in mind, if the column pins of both halves are set as the write pins,
and shared between the 2 halves, the number of write pins required is then 7.
Reading via 2 sets of 5 read pins gives us the required matrix using a total of
17 lines, leaving 2 spare.

### Other buttons.

the last 2 lines can be used to set up either a button (similar to the matrix),
or if used in conjunction with eV/GND to provide 2 buttons or indicators. This
requires more thought and research, to future TODO

### Unusables

The following pins are unusable, or not recommended for usage:
1. Pin P0.31 which is also used as Battery voltage ADC
2. Pin P0.05, as that coincides with the closed hole on the ATA connector.

This leaves us with exactly 17 usable pins out of the 19 GPIO pins.

## Matrix connections:

The shared write pins are as follows:
```
uint32_t pins_write [] = { /* P31(VBAT ADC) is separator */
  /*GPIO(31)*/GPIO(30), GPIO(27), GPIO(26),GPIO(25),
    GPIO(13), GPIO(14),/*GPIO(8),GPIO(6),*/GPIO(20),
                  /* 06 and 08 seem to be serial-only, not GPIO */
};
```
The order of the pins as connected from innermost to the outermost is 
13,14,30,27,26,25,20

The read pins are as follows:
```
uint32_t pins_read [] = {
                              GPIO(16), GPIO(15),GPIO(07),GPIO(11),/*GPIO(31)*/
 GPIO(02),GPIO(03),GPIO(04),/*GPIO(05)*/,GPIO(28),GPIO(29),GPIO(12),/*GPIO(13) is next segment*/
                    /* Hole at pin 05 is blocked on the ATA connector */
};
```
The pins 02,03,04,28,29 (in order of top to bottom) are left half, while the
right half are 16,15,07,11,12 (in order of top to bottom).

 With this in mind, the mapping of the key presses to keys can be created.
