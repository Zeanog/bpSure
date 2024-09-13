/**********************************************************************/
//	peripheral.c   July 13, 2018
/***********************************************************************

                    -----------------
      OLED_PWR  <--|3.3V           5V|<-- 5V PWR
      OLED_SCLK <--|GPIO2          5V|<-- 5V PWR
      OLED_MOSI <--|GPIO3         GND|--> UART_GND    
      OLED_SSEL <--|GPIO4      GPIO14|--> UART_TXD 
      OLED_GND  <--|GND        GPIO15|<-- UART_RXD   
 Push_Button in -->|GPIO17     GPIO18|                           
				<--|GPIO27        GND|                    
    +--SPI_DONE -->|GPIO22     GPIO23|
    |              |3.3V       GPIO24|--> Push Button LED                             
    |  SPI_MOSI <--|GPIO10        GND|                                                
    +--SPI_MISO -->|GPIO9      GPIO25|--> SPI_CE2                                     
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
#include "peripheral.h"

volatile unsigned long * p_IRQ = (unsigned long *) p_base_IRQ;
volatile unsigned long * p_GPIO = (unsigned long *) p_base_GPIO;
volatile unsigned long * p_TIMER = (unsigned long *) p_base_TIMER;

//---------------------------------------------------------------------------
int gpioMODE(unsigned int pin, unsigned int mode) {	//Set GPIO MODE
//---------------------------------------------------------------------------
	*(p_GPIO + (pin/10)) &= ~(7 << ((pin%10)*3));
	*(p_GPIO + (pin/10)) |= (mode << ((pin%10)*3));
return 0;
}

//---------------------------------------------------------------------------
void gpioWR(unsigned int pin, unsigned int state) { //Write to a GPIO pin
//---------------------------------------------------------------------------
	if(state==1) *(p_GPIO + 7) = (1 << pin);
	else   *(p_GPIO + 10) = (1 << pin);
}

//---------------------------------------------------------------------------
unsigned int gpioRD(unsigned int pin) { //Read from a GPIO pin
//---------------------------------------------------------------------------
	return ((*(p_GPIO + 13) >> pin) & 1);
}

/*
//---------------------------------------------------------------------------
void delay_ms(int ms) {
//---------------------------------------------------------------------------
   _kernel_swi_regs reg;
   ms *= 1000; //from us to ms
   reg.r[0] = ms;
   reg.r[8] = 0;
   reg.r[9] = 22;
   _kernel_swi(HAL_CounterDelay, &reg, &reg);
}

//---------------------------------------------------------------------------
void delay_us(int us) {
//---------------------------------------------------------------------------
   _kernel_swi_regs reg;
   reg.r[0] = us;
   reg.r[8] = 0;
   reg.r[9] = 22;
  _kernel_swi(HAL_CounterDelay, &reg, &reg);
}
*/
//---------------------------------------------------------------------------
unsigned int gpioPUD(unsigned int pin, unsigned char PUDstate) {
//---------------------------------------------------------------------------
   *(p_GPIO + 37) = PUDstate;
   *(p_GPIO + 38) |= (1 << pin);
//   delay_us(1);
   *(p_GPIO + 37) = 0;
   *(p_GPIO + 38) &= ~(1 << pin);
   return 0;
}

//---------------------------------------------------------------------------
unsigned int checkPinEvent(unsigned int pin) {
//---------------------------------------------------------------------------
  //uses GPEDSn registers to detect whether an event of a defined type
  //has occured.
  volatile unsigned long  *tmpGPIO;
  unsigned int pinState = 0;

  tmpGPIO = p_GPIO;

  if( pin >= 32 ) {
   tmpGPIO += GPEDS1;
   pin -=32; //Get pin number for second word.
 }
 else
   tmpGPIO += GPEDS0;

//Save the bit, write to the register to clear it,
//then return the saved register.
   pinState = (( *tmpGPIO >> pin) & 1);
   *tmpGPIO |= ( 1 << pin ); //write high to clear the event.

   return pinState;
 //return ( ( *tmpGPIO >> pin ) & 1 );

}

//---------------------------------------------------------------------------
unsigned int setPinEvent(unsigned int pin, unsigned int eventType) {
//---------------------------------------------------------------------------
   //complex function which can set an event on multiple registers.
   //They all seem to be 1 bit so it can be knocked over by checking
   //for event type and and an "if" block for choosing the
   //correct register.
   //determine pin number and register / 0 or 1
   //determine offset based on eventType
   //perform operation.

   //^^^That should yield the simplest code.
   volatile unsigned long  *tmpGPIO;
   unsigned int regBank = 0;
   //bool pinState = 0;

   tmpGPIO = p_GPIO;

   if(pin > 53)
    return HIGH; //Pin out of bounds.

   if(pin >= 32) {
      regBank = 1; //second bank
      pin -=32; //Get pin number for second word.
   }
   else
   regBank = 0; //first bank

   //now we have which bank, we need to work out the offset of the
   //correct register.

   //use a switch then apply the correct offset?

   switch (eventType) {
     case REN: {
       tmpGPIO += GPREN0;
       break;
     }
     case FEN: {
       tmpGPIO += GPFEN0;
       break;
     }
     case HEN: {
       tmpGPIO += GPHEN0;
       break;
     }
     case LEN: {
       tmpGPIO += GPLEN0;
       break;
     }
     case AREN: {
       tmpGPIO += GPAREN0;
       break;
     }
     case AFEN: {
       tmpGPIO += GPAFEN0;
       break;
     }
     default: {
       break;
     }

     tmpGPIO +=  regBank; //adds 0 or 1 for register bank.
   };// end switch()
   *tmpGPIO |= ( 1 << pin ); //set the register.
  return LOW;
}

//---------------------------------------------------------------------------
unsigned int peekGPIO(unsigned int addr) { //32 bit peek
//---------------------------------------------------------------------------
    if (addr >= 0xFFF)
        return 0;

    return *(p_GPIO + addr);
}

//---------------------------------------------------------------------------
unsigned int pokeGPIO(unsigned int addr, unsigned int val) { //32 BIT POKE
//---------------------------------------------------------------------------
    if(addr >= 0xFFF)
        return 1;

    *(p_GPIO + addr) = val;

    return 0;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

