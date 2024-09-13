/**********************************************************************/
//	kernel.c   February 21, 2022
/***********************************************************************

                    -----------------
      OLED_PWR  <--|3.3V           5V|<-- 5V PWR
      OLED_SCLK <--|GPIO2          5V|<-- 5V PWR
      OLED_MOSI <--|GPIO3         GND|--> UART_GND    
      OLED_SSEL <--|GPIO4      GPIO14|--> UART_TXD 
      OLED_GND  <--|GND        GPIO15|<-- UART_RXD   
 Push_Button in -->|GPIO17     GPIO18|                           
						 |GPIO27        GND|                    
    +--SPI_DONE -->|GPIO22     GPIO23|
    |              |3.3V       GPIO24|--> Push Button LED                             
    |  SPI_MOSI <--|GPIO10        GND|                                                
    +--SPI_MISO -->|GPIO9      GPIO25|                                     
       SPI_SCLK <--|GPIO11      GPIO8|--> SPI_CE0                                      
                   |GND         GPIO7|--> SPI_CE1
                   |ID SD       ID SC|                                                
                   |GPIO5         GND|                                  
                   |GPIO6      GPIO12|
                   |GPIO13        GND| 
                   |GPIO19     GPIO16| 
                   |GPIO26     GPIO20|
                   |GND        GPIO21|            
                    -----------------   
				Raspberry PI ZERO GPIO Header

***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "math.h"
#include "library.h"
#include "peripheral.h"
#include "OLED_display.h"

extern void PUT8(unsigned int, unsigned char);
extern unsigned char GET8(unsigned int);
extern void PUT32(unsigned int, unsigned int);
extern unsigned int GET32(unsigned int);
extern void dummy(unsigned int);
extern void disable_irq(void);
extern void enable_irq(void);
	
// foreground text colors
#define BLACK		"\x1b[0;30m"
#define GRAY		"\x1b[1;30m"
#define RED			"\x1b[1;31m"
#define GREEN		"\x1b[1;32m"
#define YELLOW		"\x1b[1;33m"
#define BLUE		"\x1b[1;34m"
#define MAGENTA	"\x1b[1;35m"
#define CYAN 		"\x1b[1;36m"
#define WHITE		"\x1b[1;37m"

#define COM_RX_Buffer_Size    1024
#define COM_TX_Buffer_Size    1024 
#define COM_TX_BUFFER_FULL  0x1F00
#define COM_RX_BUFFER_EMPTY 0x1E00

#define msb 1
#define lsb 0

//----------------------------------------------------------------------
volatile int cuff_val_processed = 0;
volatile int SW1=0;

union mic_data {
	signed char byte[2];
	signed short word;
};

volatile union mic_data mic_one;
volatile union mic_data mic_two;

//----------------------------------------------------------------------
//  COM UART buffers
//----------------------------------------------------------------------
int  COM_RX_Contents = 0, COM_TX_Contents = 0;
char COM_TX_Buffer[COM_TX_Buffer_Size];
char COM_RX_Buffer[COM_RX_Buffer_Size];

char *Begin_COM_RX_Pointer = COM_RX_Buffer;
char *End_COM_RX_Pointer = COM_RX_Buffer + COM_RX_Buffer_Size-1;
char *Write_COM_RX_Pointer = COM_RX_Buffer;
char *Read_COM_RX_Pointer = COM_RX_Buffer;

char *Begin_COM_TX_Pointer = COM_TX_Buffer;
char *End_COM_TX_Pointer = COM_TX_Buffer + COM_TX_Buffer_Size-1;
char *Write_COM_TX_Pointer = COM_TX_Buffer;
char *Read_COM_TX_Pointer = COM_TX_Buffer;

RingBuffer	pulse_data;
PulseInfo 	pulse;

//----------------------------------------------------------------------
//	UART System Initialization
//----------------------------------------------------------------------
void uart_init(void) {
    unsigned int ra;
    //alt function 5 for uart1
	 gpioMODE(14, ALT5); //GPIO14 TXD
	 gpioMODE(15, ALT5); //GPIO15 RXD

    PUT32(AUX_ENABLES,1);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
	 PUT32(AUX_MU_IER_REG, 0);
    PUT32(AUX_MU_IIR_REG,0xC6);
    PUT32(AUX_MU_BAUD_REG,270);
 
    PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,(3<<14));
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,0);
    PUT32(AUX_MU_CNTL_REG,3);
}
//----------------------------------------------------------------------
int uart_getc(void) {
	int char_in;
	if(Read_COM_RX_Pointer != Write_COM_RX_Pointer) {
		char_in = *Read_COM_RX_Pointer;
		if(++Read_COM_RX_Pointer > End_COM_RX_Pointer)
			Read_COM_RX_Pointer = Begin_COM_RX_Pointer;
	}
	else char_in = COM_RX_BUFFER_EMPTY;
	return char_in;
}
//----------------------------------------------------------------------
int uart_putc(int char_out) {
	COM_TX_Contents = Write_COM_TX_Pointer - Read_COM_TX_Pointer;
	if(COM_TX_Contents < 0)
		COM_TX_Contents = COM_TX_Buffer_Size + COM_TX_Contents;    
	if(COM_TX_Contents < (COM_TX_Buffer_Size - 1)) {        
		*Write_COM_TX_Pointer = char_out;
		if(++Write_COM_TX_Pointer > End_COM_TX_Pointer)
			Write_COM_TX_Pointer = Begin_COM_TX_Pointer;
		return RETURN_SUCCESS;
	}
	else return COM_TX_BUFFER_FULL;
}
//----------------------------------------------------------------------
void uart_puts(char *s) {
	while(*s)
		uart_putc(*s++);
}
//----------------------------------------------------------------------
//	SPI System Initialization
//----------------------------------------------------------------------
void spi_init(void) {
	gpioMODE(7,  OUTPUT);	//CE1
	gpioMODE(8,  OUTPUT);	//CE0
	gpioMODE(9,  ALT0);		//MISO
	gpioMODE(10, ALT0);		//MOSI
	gpioMODE(11, ALT0);		//SCLK
	gpioMODE(22, INPUT);	//MAX187 DOUT Sense
	gpioMODE(25, OUTPUT);	//CE2 - MAX187
	
	gpioWR(7,  HIGH);
	gpioWR(8,  HIGH);
	gpioWR(25, HIGH);

	PUT32(SPI_CLK, 0x0000003E);	//4Mhz SPI clock
}
//----------------------------------------------------------------------
//	get microphone data
//----------------------------------------------------------------------
int spi_microphones() {
	 gpioWR(7, LOW);					//CE1 chip enable		
    PUT32(SPI_CS, 0x000000B4);		//clear fifo registers & set TA=1	
    while(!(GET32(SPI_CS) & 0x00040000)) continue;
    PUT32(SPI_FIFO, 0x81);
 	 while(!(GET32(SPI_CS) & 0x00010000)) continue;
    mic_one.byte[msb] = GET8(SPI_FIFO) & 0xFF;    
    while(!(GET32(SPI_CS) & 0x00040000)) continue;
    PUT32(SPI_FIFO, 0xEB);
    while(!(GET32(SPI_CS) & 0x00010000)) continue;
    mic_one.byte[lsb] = GET8(SPI_FIFO) & 0xFF;
    PUT32(SPI_CS, 0x00000000);		//set TA=0
    gpioWR(7, HIGH);				//CE1 chip disable
    
/////////////////////////////////////
	
	 gpioWR(8, LOW);					//CE0 chip enable		
    PUT32(SPI_CS, 0x000000B4);		//clear fifo registers & set TA=1	
    while(!(GET32(SPI_CS) & 0x00040000)) continue;
    PUT32(SPI_FIFO, 0x81);
 	 while(!(GET32(SPI_CS) & 0x00010000)) continue;
    mic_two.byte[msb] = GET8(SPI_FIFO) & 0xFF;    
    while(!(GET32(SPI_CS) & 0x00040000)) continue;
    PUT32(SPI_FIFO, 0xEB);
    while(!(GET32(SPI_CS) & 0x00010000)) continue;
    mic_two.byte[lsb] = GET8(SPI_FIFO) & 0xFF;
    PUT32(SPI_CS, 0x00000000);		//set TA=0
    gpioWR(8, HIGH);				//CE0 chip disable

   return 0;
}

float process_microphones() {
	float mic_one_sig = (float)mic_one.word;
	float mic_two_sig = (float)mic_two.word;
	
	float val = (mic_one_sig * mic_two_sig);
	return val < 0 ? 0 : val;
}

//----------------------------------------------------------------------
//	get cuff pressure
//----------------------------------------------------------------------
int spi_cuff_pressure(void) {	
	PUT32(SPI_CS, 0x000000B0);	//clear fifo registers & set TA=1	
	gpioWR(25, LOW);				//CE2 chip enable
	while(gpioRD(22) == 0) continue;
	while(!(GET32(SPI_CS) & 0x00040000)) continue;
	PUT32(SPI_FIFO, 0x00);
	while(!(GET32(SPI_CS) & 0x00010000)) continue;
	int cuff_raw = GET8(SPI_FIFO) & 0x7F;     
	while(!(GET32(SPI_CS) & 0x00040000)) continue;
	PUT32(SPI_FIFO, 0x00);
	while(!(GET32(SPI_CS) & 0x00010000)) continue;
	cuff_raw = (cuff_raw << 5) + (GET8(SPI_FIFO) >> 3);
	PUT32(SPI_CS, 0x00000000);	//set TA=0
	gpioWR(25, HIGH);				//CE2 chip disable
	cuff_raw -= 445;
	if(cuff_raw < 0) cuff_raw = 0;
	return cuff_raw;
}
//----------------------------------------------------------------------
// interrupt callback routine for the COM UART receive register
//----------------------------------------------------------------------
void UART_RX(void) {
	*Write_COM_RX_Pointer = GET8(AUX_MU_IO_REG);
	if(++Write_COM_RX_Pointer > End_COM_RX_Pointer)
		Write_COM_RX_Pointer = Begin_COM_RX_Pointer;
}
//----------------------------------------------------------------------
// interrupt callback routine for the COM UART transmit register
//----------------------------------------------------------------------
void UART_TX(void) {
	if(Read_COM_TX_Pointer != Write_COM_TX_Pointer) {
		PUT8(AUX_MU_IO_REG, *Read_COM_TX_Pointer);
		if(++Read_COM_TX_Pointer > End_COM_TX_Pointer)
			Read_COM_TX_Pointer = Begin_COM_TX_Pointer;
	}
}

//----------------------------------------------------------------------
void irq_service_routine(void) {
    unsigned int irq_pending1;
    static unsigned int push_button = 0;
    static unsigned int OLED_display = 0;
    static unsigned int cuff_pressure = 0;
    
    static char cuff_buff[20];
    cuff_buff[19] = '\0';

    irq_pending1=GET32(IRQ_PEND1);
	 if(irq_pending1&2) {		
		PUT32(C1,(GET32(CLO) + 0x000004E1)); 	//increment the counter
		PUT32(CS,2);  					  	//clear the timer interrupt
		
		if(!(push_button++ % 32)) { 	//latching push button
			static uint8_t SR1 = 0;
			SR1 = (SR1 << 1) | !gpioRD(17);
			if(SR1 == 0x80) SW1 ^= 1;
		}
	
////////////////////////////////////////
		spi_microphones();		//get data from the microphones
		
		int mic_val = (int)process_microphones();
		WriteToRingBuffer( &pulse_data, mic_val);
		
		//int signals[RINGBUFFER_SIZE];
		//thresholding(pulse_data.Buffer, signals, 1, 50, 0.5f);
		int signalEnd = DetermineDeviation(&pulse_data);
		//for(int ix = 1; ix < RINGBUFFER_SIZE; ++ix) {
		//	if( signals[ix - 1] > signals[ix] ) {
		//		signalEnd = 1;
		//		break;
		//	}
		//}
					
		if(!(cuff_pressure++ % 80)) {
			cuff_val_processed = spi_cuff_pressure()*10 / 135;
		}
////////////////////////////////////////
		
		if(!(OLED_display++ % 400)) {
			OLED_pos(1, 2);
			OLED_puts("bpSure Monitor");
			OLED_pos(2, 1);
			OLED_puts("Cuff Press =    ");
			OLED_pos(2, 13);
			itos(cuff_buff, signalEnd, 3);
			OLED_puts(cuff_buff);
			OLED_pos(2, 13);
		}
	}
}

//----------------------------------------------------------------------
void _main_ (unsigned int earlypc) {
//----------------------------------------------------------------------
   disable_irq();

   PUT32(IRQ_DISABLE1, 0xFF); 
   PUT32(IRQ_DISABLE2, 0xFF);
	PUT32(IRQ_DISABLE_BASIC, 0xFF);
    
	gpioMODE(17, INPUT);	// front panel switch in
	gpioMODE(24, OUTPUT);	// front panel switch out        

	unsigned int STAT_REG;
		
    //UART init
	uart_init();
   GET8(AUX_MU_IO_REG);
    
   //SPI init
   spi_init();
    	    
	//clock init
	PUT32(C1,(GET32(CLO) + 0x000004E1));
	PUT32(CS,2);
	PUT32(IRQ_ENABLE1, 2);
	enable_irq();

	//OLED init
	OLED_init();
	
	OLED_pos(1, 2);
	OLED_puts("bpSure Monitor");
	OLED_pos(2, 1);
	OLED_puts("                  ");

	InitRingBuffer(&pulse_data);

	while(1) {
		gpioWR(24, SW1);
		STAT_REG = GET32(AUX_MU_STAT_REG);
		
		if(STAT_REG & (1<<0)) { 	//recr FIF0 contains one byte
			UART_RX();
		}

		if(STAT_REG & (1<<1)) { 	//xmtr FIFO can accept one byte
			UART_TX();
		}
	}
}

//----------------------------------------------------------------------
//								END OF FILE
//----------------------------------------------------------------------
