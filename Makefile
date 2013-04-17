TARGET = stackdemo
CPU=msp430f5510
SRCS=$(TARGET).c
FET = rf2500
FCPU=8000000

CC = msp430-gcc
OBJDUMP = msp430-objdump
SIZE = msp430-size
MSPDEBUG = mspdebug
CFLAGS = -std=gnu99 -Os -Wall -mmcu=$(CPU) -DFCPU=$(FCPU)
GDB = msp430-gdb

SRCS+=hardware.c spi.c
SRCS+=stack/enc28j60.c stack/dhcp_client.c stack/dnslkup.c stack/ip_arp_udp_tcp.c stack/websrv_help_functions.c

# Uncomment the following to enable debugging
#CFLAGS += -g -DDEBUG

OBJS=$(SRCS:.c=.o)

all: $(TARGET).elf  $(TARGET).lst
	$(SIZE) $(TARGET).elf

.PHONY: all

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET).elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.lst: %.elf
	$(OBJDUMP) -DS $< >$@

clean:
	rm -fr $(TARGET).elf $(TARGET).lst $(OBJS)

install: $(TARGET).elf
	$(MSPDEBUG) $(FET) "prog $(TARGET).elf"

mspdebug: $(TARGET).elf
	$(MSPDEBUG) $(FET)

debug: $(TARGET).elf
	$(MSPDEBUG) $(FET) gdb

gdb: $(TARGET).elf
	$(GDB) $(TARGET).elf