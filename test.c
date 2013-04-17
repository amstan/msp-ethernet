#include "hardware.h"
#include "spi.h"

void test(void) {
	//hello world
}
void (*app_entry_point)(void)=*test;

void tick(void) {
	toggle_bit(P_OUT(LED_P),LED);
}
void (*system_tick)(void)=*tick;