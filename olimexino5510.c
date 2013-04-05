#include <msp430.h>
#include "bitop.h"

void initio() {
	#define LED 3
	#define UEXTPWR 1
	#define CS 0
	PJDIR=(1<<LED)|(1<<UEXTPWR)|(1<<CS);
	PJOUT=(0<<UEXTPWR)|(1<<CS);
	
	#define CLOCK 3
	#define DOUT 1
	#define DIN 2
	P4DIR=(1<<CLOCK)|(1<<DOUT);
	
	#define BUTTON 0
	P2OUT=(1<<BUTTON);
	P2REN=(1<<BUTTON);
	#define switchpressed() (!test_bit(P2IN,BUTTON))
}

//Enables spi communication, 1 for start, 0 for stop
void spi_enable(unsigned char enable) {
	change_bit(PJOUT,CS,!enable);
}

void clockpulse(unsigned char rising) {
	//Switch here!
	change_bit(P4OUT,CLOCK,rising);
	__delay_cycles(20);
	
	change_bit(P4OUT,CLOCK,!rising);
	__delay_cycles(20);
}

void spi_send(unsigned char data) {
	for(signed char bit=7;bit>=0;bit--) {
		change_bit(P4OUT,DOUT,test_bit(data,bit));
		clockpulse(1);
	}
}

unsigned char spi_recieve(void) {
	unsigned char data=0;
	for(signed char bit=7;bit>=0;bit--) {
		clockpulse(1);
		change_bit(data,bit,test_bit(P4IN,DIN));
	}
	return data;
}

unsigned char spi_send_recieve(unsigned char data) {
	unsigned char recievedata=0;
	for(signed char bit=7;bit>=0;bit--) {
		change_bit(P4OUT,DOUT,test_bit(data,bit));
		change_bit(recievedata,bit,test_bit(P4IN,DIN));
		clockpulse(1);
	}
	return recievedata;
}

#define RCR 0b00000000
#define WCR 0b01000000

void spi_write(unsigned char reg, unsigned char data) {
	spi_enable(1);
	
	//send register to write
	spi_send_recieve(WCR+reg);
	
	//send data to write in register
	spi_send_recieve(data);
	
	spi_enable(0);
}

unsigned char spi_read(unsigned char reg) {
	spi_enable(1);
	
	//send register to write
	spi_send_recieve(RCR+reg);
	
	//recieve data to write in register
	unsigned char data=spi_send_recieve(0);
	
	spi_enable(0);
	return data;
}