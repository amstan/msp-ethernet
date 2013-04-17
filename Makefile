TARGET = ledblink
CPU=msp430f5510
SRCS=$(TARGET).c
FET = rf2500
FCPU=8000000
HTMLs=index.html
AUTH=alex:pass

AUTHSTRING=$(shell python2 -c 'import base64; print base64.b64encode("$(AUTH)")')
CC = msp430-gcc
OBJDUMP = msp430-objdump
SIZE = msp430-size
MSPDEBUG = mspdebug
CFLAGS = -std=gnu99 -O2 -Wall -mmcu=$(CPU) -DFCPU=$(FCPU) -DAUTH=\"$(AUTHSTRING)\"
GDB = msp430-gdb

#add hardware stuff
SRCS+=hardware.c spi.c

#add ethernet stack
SRCS+=tgNetStack/enc28j60.c tgNetStack/dhcp_client.c tgNetStack/dnslkup.c tgNetStack/ip_arp_udp_tcp.c tgNetStack/websrv_help_functions.c

# Uncomment the following to enable debugging
CFLAGS += -g -DDEBUG

OBJS=$(SRCS:.c=.o)
OBJS+=html.h

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

install: all
	$(MSPDEBUG) $(FET) "prog $(TARGET).elf"

mspdebug: $(TARGET).elf
	$(MSPDEBUG) $(FET)

debug: $(TARGET).elf
	$(MSPDEBUG) $(FET) gdb

gdb: $(TARGET).elf
	$(GDB) $(TARGET).elf

html.h: $(HTMLs)
	./generatehtmlheader.py $(HTMLs)>html.h

$(TARGET).c: html.h

test:
	echo $(AUTHSTRING)