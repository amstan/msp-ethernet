#include "hardware.h"
#include "spi.h"

void test(void) {
	spi_init(UCCKPL*0+UCCKPH*1); //Clock idle in a low state, data on rising edge
	
	static unsigned char counter;
	while(1) {
		counter++;
		unsigned char data;
		spi_enable(1);
		data=spi_send_recieve(counter);
		spi_enable(0);
		
		change_bit(P_OUT(LED_P),LED,data==counter);
	}
}
void (*app_entry_point)(void)=*test;

void tick(void) {
}
void (*system_tick)(void)=*tick;