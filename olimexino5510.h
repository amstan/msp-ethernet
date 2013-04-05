#include <msp430.h>
#include "bitop.h"

#define LED 3
#define UEXTPWR 1
#define CS 0

#define CLOCK 3
#define DOUT 1
#define DIN 2


#define BUTTON 0
#define switchpressed() (!test_bit(P2IN,BUTTON))

void initio();

//Enables spi communication, 1 for start, 0 for stop
void spi_enable(unsigned char enable);

void clockpulse(unsigned char rising);

void spi_send(unsigned char data);

unsigned char spi_recieve(void);

unsigned char spi_send_recieve(unsigned char data);

#define RCR 0b00000000
#define WCR 0b01000000

void spi_write(unsigned char reg, unsigned char data);

unsigned char spi_read(unsigned char reg);