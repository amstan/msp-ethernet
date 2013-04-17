#include "hardware.h"
#include "spi.h"
#include "tgNetStack/tgNetStack.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "html.h"

#define TG_BUFFER_SIZE 2048
static uint8_t buf[TG_BUFFER_SIZE+1];

void blinkled(void) {
	toggle_bit(P_OUT(LED_P),LED);
}
void (*system_tick)(void)=*blinkled;

void httpServer(void) {
	//Prepare spi for the ethernet chip
	spi_init(UCCKPL*0+UCCKPH*1); //Clock idle in a low state, data on rising edge
	
	uint8_t MAC_ADDR[6] = {0x00,0xDE,0xAD,0xBE,0xEF,0x00};
	uint8_t IP[4] = {192,168,0,2};
	tgNetStackInit(MAC_ADDR,IP);
	
	#define pgprintf(...) {sprintf(buffer,__VA_ARGS__); pos=tgPageAdd(buf, pos, buffer);}
	char buffer[64]; //buffer for pgprintf
	
	unsigned long hitcounter=0;
	
	uint16_t pos;
	while(1) {
		//Wait for valid TCP packet
		if (!(pos = tgPacketGet(buf, TG_BUFFER_SIZE))) {
			//empy packet
			continue;
		} else if (strncmp("GET ", (char *) &buf[pos], 4)) {
			//Non GET Cmds
			pos = tgPageAdd(buf, pos, HTTP_RESP_OK);
			pos = tgPageAdd(buf, pos, HTML_RESP_OK);
			tgHttpReply(buf, pos);
			continue;
		}
		
		hitcounter++;
		
		#define if_page(name) if(strncmp(name, (char *) &buf[pos + 4], strlen(name))==0)
		
		if_page("/uptime ") {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,HTML_REFRESH_HEADER);
			pgprintf("Uptime: %ldh %dm %ds",uptime.hours, uptime.minutes, uptime.seconds);
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		else if_page("/hitcounter ") {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,HTML_REFRESH_HEADER);
			pgprintf("Hits: %ld", hitcounter);
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		else if_page("/ ") {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,index_html);
		}
		else if_page("/test ") {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			for(unsigned int i=0;i<index_html_len;i++) {
				pgprintf("%c",index_html[i]);
			}
			//pos=tgPageAdd(buf,pos,index_html+0);
		}
		else if_page("/ledon ") {
			set_bit(P_OUT(LED_P),LED);
			pos=tgPageAdd(buf,0,HTTP_RESP_REDIRECT);
		}
		else if_page("/ledoff ") {
			clear_bit(P_OUT(LED_P),LED);
			pos=tgPageAdd(buf,0,HTTP_RESP_REDIRECT);
		}
		else if_page("/blinkon ") {
			system_tick=*blinkled;
			pos=tgPageAdd(buf,0,HTTP_RESP_REDIRECT);
		}
		else if_page("/blinkoff ") {
			system_tick=0;
			pos=tgPageAdd(buf,0,HTTP_RESP_REDIRECT);
		}
		else {
			//404
			pos=tgPageAdd(buf,0,HTTP_RESP_NOT_FOUND);
			pos=tgPageAdd(buf,pos,HTML_RESP_NOT_FOUND);
			pos=tgPageAdd(buf,pos,"<a href=\"/\">Back to main page</a>");
		}
		
		tgHttpReply(buf,pos);
	}
}
void (*app_entry_point)(void)=*httpServer;