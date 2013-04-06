# Makefile created by gnomad

PROG = main
CC = msp430-gcc -g
CXX = msp430-g++
OBJDUMP = msp430-objdump
SIZE = msp430-size
MSPDEBUG = mspdebug
CFLAGS = --std=c99 -Os -Wall -mmcu=msp430f5510
FET = rf2500
GDB = msp430-gdb
GDBTUI = $(GDB)tui

OBJS=$(PROG).o olimexino5510.o
OBJS+=stack/enc28j60.o stack/dhcp_client.o stack/dnslkup.o stack/ip_arp_udp_tcp.o stack/websrv_help_functions.o

all: $(PROG).elf  $(PROG).lst
	$(SIZE) $(PROG).elf

.PHONY: all

$(PROG).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG).elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.lst: %.elf
	$(OBJDUMP) -DS $< >$@

clean:
	rm -fr $(PROG).elf $(PROG).lst $(OBJS)

install: $(PROG).elf
	$(MSPDEBUG) $(FET) "prog $(PROG).elf"

mspdebug: $(PROG).elf
	$(MSPDEBUG) $(FET)

debug: $(PROG).elf
	$(MSPDEBUG) $(FET) gdb

gdb: $(PROG).elf
	$(GDB) $(PROG).elf
 
tui: $(PROG).elf
	$(GDBTUI) $(PROG).elf
 
ddd: $(PROG).elf
	ddd --debugger $(GDB) $(PROG).elf