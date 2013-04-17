#pragma once
#include "hardware.h"
#include "bitop.h"

///Initializes spi peripherals and pins
void spi_init(unsigned int clocksettings);

///Enables spi communication, 1 for start, 0 for stop
void spi_enable(unsigned char enable);

///Sends byte and returns with recieved byte
unsigned char spi_send_recieve(unsigned char send);