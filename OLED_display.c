/**********************************************************************/
//	OLED_display.c   July 11, 2017
/***********************************************************************
            Program to write to the Adafruit 16x2 OLED Display

                    July 11, 2017   Dennis Dahr, PhD

------------------------------------------------------------------------
               0   0   1   1   0   0   1   0   0   1
                       _______         ___         ______________
        SDO           |       |       |   |       |
        ______________|       |_______|   |_______|
        ______   _   _   _   _   _   _   _   _   _   ____________
        SCK   | | | | | | | | | | | | | | | | | | | |
              |_| |_| |_| |_| |_| |_| |_| |_| |_| |_|
               ^                                   ^
              SPI_data_0                      SPI_data_9
        _____   (msb)                           (lsb)      _____
        STB  |                                            |
             |____________________________________________|

              Set DDRAM Address to 0x49 using SPI MODE #3

----------------------------------------------------------------------*/
#include "OLED_display.h"
#include "peripheral.h"

extern void dummy (unsigned int);

#define SCLK  2
#define MOSI  3
#define SSEL  4

void OLED_command(int cmd) {
    int  command = 0x0000 | cmd;
    gpioWR(SSEL, 0);
    for(int i=9; i>=0; i--) {
        gpioWR(SCLK, LOW);
        gpioWR(MOSI, (command & (1 << i)) >> i);
        gpioWR(SCLK, HIGH);
    }
    gpioWR(MOSI, 0);
    gpioWR(SSEL, 1);
    gpioWR(SCLK, 0);
}

void OLED_putc(int chr) {
     int data = 0x0200 | chr;
     gpioWR(SSEL, 0);
     for(int i=9; i>=0; i--) {
        gpioWR(SCLK, LOW);
        gpioWR(MOSI, (data & (1 << i)) >> i);
        gpioWR(SCLK, HIGH);
    }
    gpioWR(MOSI, 0);
    gpioWR(SSEL, 1);
    gpioWR(SCLK, 0);
}

void OLED_puts(char *s) {
    while(*s)
        OLED_putc(*s++);
}

void OLED_pos(int row, int col) {
    char Row_Addr[] = { 0x00, 0x40 };
    char  First_Char_Addr = Row_Addr[row-1] + col-1 + 0x80;
    OLED_command(First_Char_Addr);
}

void OLED_init() {
     gpioMODE(MOSI, OUTPUT);
     gpioMODE(SCLK, OUTPUT);
     gpioMODE(SSEL, OUTPUT);
     gpioWR(MOSI, 0);
     gpioWR(SCLK, 0);
     gpioWR(SSEL, 1);
    
     OLED_command(0x38); // function set
     OLED_command(0x0C); // display ON and cursor OFF
     OLED_command(0x01); // clear display
     OLED_command(0x02); // go home
     OLED_command(0x06); // entry mode set
     OLED_command(0x80); // set DDRAM address to 0
     
     for(int ra=0; ra<10000; ra++)
		 __asm__("nop");
}

