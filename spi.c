#include "hardware.h"
#include "spi.h"
#include "hardware.h"
#include "bitop.h"

void spi_init(unsigned int clocksettings) {
	//setup CS pin
	set_bit(P_DIR(CS_P),CS);
	set_bit(P_OUT(CS_P),CS);
	
	//giveup control of the other pins to the spi hardware
	P_SEL(SPI_P)=(1<<CLOCK)|(1<<DOUT)|(1<<DIN);
	
	// Put state machine in reset
	UCB1CTL1 = UCSWRST;
	
	// 3-pin, 8-bit SPI master, MSB
	UCB1CTL0 = UCMST+UCSYNC+UCMSB+clocksettings;
	
	// SMCLK
	UCB1CTL1 = UCSSEL_2;
	
	//No prescaler
	UCB1BR0 = 0;
	UCB1BR1 = 0;
	
	//Start SPI hardware
	clear_bits(UCB1CTL1,UCSWRST);
}

void spi_enable(unsigned char enable) {
	change_bit(P_OUT(CS_P),CS,!enable);
}

unsigned char spi_send_recieve(unsigned char send) {
	UCB1TXBUF=send;
	
	///wait until it recieves
	while (!(UCB1IFG&UCRXIFG));
	
	return UCB1RXBUF;
}