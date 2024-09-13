/**********************************************************************/
//	peripheral.h   September 25, 2018
/**********************************************************************/
//address defines
#define p_base_IRQ    0x2000B200
#define p_base_GPIO   0x20200000
#define p_base_TIMER  0x20003000

//FSEL pin function defines
#define INPUT    0b000
#define OUTPUT   0b001
#define ALT0     0b100
#define ALT1     0b101
#define ALT2     0b110
#define ALT3     0b111
#define ALT4     0b011
#define ALT5     0b010

//GPIO base address defines
#define RPI0     0x20200000
#define RPI1     0x20200000
#define RPI2     0x3F200000
#define RPI3     0x3F200000

//pullup state defines
#define PUDOFF   0b00
#define PUDDOWN  0b01
#define PUDUP    0b10
#define PUDRSVD  0b11  //reserved

//GPIO Pin event types
#define REN      0 //Rising Edge detect enable
#define FEN      1 //Falling Edge detect enable
#define HEN      2 //High Detect Enable
#define LEN      3 //Low Detect Enable
#define AREN     4 //Async Rising Edge Detect Enable
#define AFEN     5 //Async Falling Edge Detect Enable

//I/O physical addresses
#define GPFSEL0  0x20200000
#define GPFSEL1  0x20200004
#define GPFSEL2  0x20200008
#define GPFSEL3  0x2020000C
#define GPFSEL4  0x20200010
#define GPFSEL5  0x20200014

#define GPSET0   0x2020001C
#define GPSET1   0x20200020

#define GPCLR0   0x20200028
#define GPCLR1   0x2020002C

#define GPLEV0   0x20200034
#define GPLEV1   0x20200038

#define GPEDS0   0x20200040
#define GPEDS1	 0x20200044

#define GPREN0   0x2020004C
#define GPREN1   0x20200050

#define GPFEN0   0x20200058
#define GPFEN1   0x2020005C

#define GPHEN0   0x20200064
#define GPHEN1   0x20200068

#define GPLEN0   0x20200070
#define GPLEN1   0x20200074

#define GPAREN0  0x2020007C
#define GPAREN1  0x20200080

#define GPAFEN0  0x20200088
#define GPAFEN1  0x2020008C

#define GPPUD     0x20200094
#define GPPUDCLK0 0x20200098
#define GPPUDCLK1 0x2020009C

//UART physical addresses
#define AUX_ENABLES     0x20215004
#define AUX_MU_IO_REG   0x20215040
#define AUX_MU_IER_REG  0x20215044
#define AUX_MU_IIR_REG  0x20215048
#define AUX_MU_LCR_REG  0x2021504C
#define AUX_MU_MCR_REG  0x20215050
#define AUX_MU_LSR_REG  0x20215054
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

//TIMER physical addresses
#define CS	0x20003000
#define CLO 0x20003004
#define CHI 0x20003008
#define C0  0x2000300C
#define C1  0x20003010
#define C2  0x20003014
#define C3  0x20003018

//SPI physical addresses
#define SPI_CS	 0x20204000
#define SPI_FIFO 0x20204004
#define SPI_CLK  0x20204008

//IRQ physical addresses
#define IRQ_BASIC 		  0x2000B200
#define IRQ_PEND1 		  0x2000B204
#define IRQ_PEND2 		  0x2000B208
#define IRQ_FIQ_CONTROL   0x2000B210
#define IRQ_ENABLE1 	  0x2000B210
#define IRQ_ENABLE2 	  0x2000B214
#define IRQ_ENABLE_BASIC  0x2000B218
#define IRQ_DISABLE1 	  0x2000B21C
#define IRQ_DISABLE2	  0x2000B220
#define IRQ_DISABLE_BASIC 0x2000B224

// pin states
#define LOW  0
#define HIGH 1
#define SET 1
#define CLR 0

void enable_irq(void);
void disable_irq(void);
unsigned int mem_map(unsigned int);

/***********************************************************************
			Sets pin mode
INPUT    0b000			ALT2	0b110
OUTPUT   0b001			ALT3	0b111
ALT0     0b100			ALT4	0b011
ALT1     0b101			ALT5	0b010
***********************************************************************/
int gpioMODE(unsigned int pin, unsigned int mode);

/***********************************************************************
Write to the chosen pin. State is HIGH or LOW
***********************************************************************/
void gpioWR(unsigned int pin, unsigned int state);

/***********************************************************************
Reads the state of the pin. Returns HIGH or LOW.
Pin must be set to an input first.
This function does not change the pin mode.
***********************************************************************/
unsigned int gpioRD(unsigned int pin);

unsigned int setPUD(unsigned int pin, unsigned char PUDState);
/*
Sets pullup on pins to up, down or off with PUDOFF, PUDDOWN and PUDUP
 */

unsigned int checkPinEvent(unsigned int pin);
/*
Checks whether an event of the specified type has occurred on the pin.
Calling this clears the event ready for next time.
*/

unsigned int setPinEvent(unsigned int pin, unsigned int eventType);
/*
Sets a pin to capture a certain event type.
Event types are:
REN  - Rising edge
FEN  - Falling edge
HEN  - High enable
LEN  - Low enable
AREN - Async rising edge (triggers on events < 1 clock)
AFEN - Async falling edge. As above.

returns a high state if pin number is out of bounds.

The event triggers are checked with checkPinEvent
*/

