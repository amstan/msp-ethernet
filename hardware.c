#include <msp430.h>
#include "hardware.h"
#include "bitop.h"

void wtd_disable() {
	WDTCTL = WDTPW + WDTHOLD;
}

void wtd_interrupt_setup(unsigned int frequency) {
	SFRIE1 |= WDTIE; //Watchdog interrupts
	WDTCTL = WDTTMSEL + frequency;
}

time_t uptime;
void __attribute__((interrupt (WDT_VECTOR))) second_interrupt() {
	if(uptime.seconds >= 59) {
		uptime.seconds = 0;
		if(uptime.minutes >= 59) {
			uptime.minutes = 0;
			uptime.hours++;
		} else {
			uptime.minutes++;
		}
	} else {
		uptime.seconds++;
	}
	
	if(system_tick) {
		system_tick();
	}
}

void io_init() {
	//Set led to output
	set_bit(P_DIR(LED_P),LED);
	
	//Power UEXT board
	set_bit(P_DIR(UEXTPWR_P),UEXTPWR);
	clear_bit(P_OUT(UEXTPWR_P),UEXTPWR);
	
	//button
	clear_bit(P_DIR(BUTTON_P),BUTTON);
	set_bit(P_OUT(BUTTON_P),BUTTON);
	set_bit(P_REN(BUTTON_P),BUTTON);
}

void core_frequency_set(unsigned long int frequency) {
	///Set multiplier based on the slow xtal
	UCSCTL3 |= SELREF_2;
	UCSCTL4 |= SELA_2;
	UCSCTL0 = 0x0000;
	UCSCTL1 = DCORSEL_5;
	UCSCTL2 = FLLD_1 + (frequency/32768)-1;
}

int main(void) {
	//start watchdog for uptime counting
	wtd_interrupt_setup(WDT_ARST_1000); //auxiliary 32kHz xtal divided to 1Hz
	
	core_frequency_set(FCPU);
	
	io_init();
	
	//enable global interrupts
	__bis_SR_register(GIE);
	
	//Run application code
	if(app_entry_point) {
		app_entry_point();
	}
	
	while(1);
	return 0;
}