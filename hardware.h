#pragma once
#include <msp430.h>
#include "bitop.h"

///Stops watchdog timer
void wtd_disable();

///Application Entry Point
void (*app_entry_point)(void);

///System Tick Action
///Executes every second
void (*system_tick)(void);

///Time Struct
typedef struct {
	unsigned int hours;
	unsigned char minutes;
	unsigned char seconds;
} time_t;

extern time_t uptime;

//Macros for ports
#define CONCATENATE(x, y) x ## y
#define P_DIR(PORT)  CONCATENATE(PORT, DIR)
#define P_IN(PORT)   CONCATENATE(PORT, IN)
#define P_OUT(PORT)  CONCATENATE(PORT, OUT)
#define P_REN(PORT)  CONCATENATE(PORT, REN)
#define P_SEL(PORT)  CONCATENATE(PORT, SEL)
#define P_SEL2(PORT) CONCATENATE(PORT, SEL2)

//Olimexino5510 specific stuff
#define LED_P PJ
#define LED 3

#define UEXTPWR_P PJ
#define UEXTPWR 1

#define CS_P PJ
#define CS 0

#define SPI_P P4
#define CLOCK 3
#define DOUT 1
#define DIN 2

#define BUTTON_P P2
#define BUTTON 0
#define switchpressed() (!test_bit(P_IN(BUTTON_P),BUTTON))