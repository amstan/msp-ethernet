#Olimexino5510 Core files
##Alexandru Stan

###Features
* Entry points(via function pointers) for application, second
  interrupt(system_tick), button press down
* System tick using watchdog timer(clocked from the 32k xtal)
* Runtime selectable core frequency
* Portable pin/port definitions via P_* macros
* System uptime
* SPI Driver
* tgNetStack
	* HTTP Authentication(password generated in the Makefile)
	* HTML file includes via the Makefile

###Note
Unfortunatelly, due to different register names these core files are not
compatible with the MSP430G value line. The changes should be minimal, but
they're extensive throughout the code.

Milisecond accuracy delays were left out to keep timers free. Main system
timer runs at 1s accuracy and can be used for long timekeeping. For anything
shorter it is recommended to actually get a custom timer setup or use
__delay_cycles() for intervals that don't matter as much.

This folder is actually a git repository. Feel free to browse around.