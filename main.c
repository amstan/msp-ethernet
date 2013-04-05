#include <msp430.h>
#include "bitop.h"
#include "olimexino5510.h"

#define MIREGADR 0x14
#define MIWRL 0x16
#define MIWRH 0x17
#define MIRDL 0x18
#define MIRDH 0x19
#define MICMD 0x12
#define PHLCON 0x14
#define ECON1 0x1F

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
	
	initio();
	
	while(1) {
		//clear_bit(PJOUT,LED);
		unsigned on=switchpressed();
		//on*=((phase++)%2)==0;
		spi_write(ECON1,2); //select bank 2
		spi_write(MIREGADR,PHLCON);
		spi_write(MIWRL,0b10000000+0b00010000*(!on));
		spi_write(MIWRH,0b00001000+0b00000001*(!on));
		change_bit(PJOUT,LED,spi_read(MIWRH)==(0b00001000+0b00000001));
		__delay_cycles(10000);
	}
}