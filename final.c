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

unsigned char is_valid_auth(uint8_t *buf, char *auth) {
	char *authstart=auth;
	for(unsigned int i=0;i<500;i++) {
		if(*buf++==(uint8_t)*auth) {
			//continue checking
			auth++;
		} else {
			//go back from scratch
			auth=authstart;
		}
		if(*auth=='\0')
			return 1;
	}
	return 0;
}

volatile unsigned char button_presses=0;
void increment_button_presses(void) {
	button_presses++;
	button_presses%=10;
}
void (*button_down)(void)=*increment_button_presses;

void final(void) {
	//Prepare spi for the ethernet chip
	spi_init(UCCKPL*0+UCCKPH*1); //Clock idle in a low state, data on rising edge
	
	uint8_t MAC_ADDR[6] = {0x00,0xDE,0xAD,0xBE,0xEF,0x00};
	uint8_t IP[4] = {192,168,0,55};
	tgNetStackInit(MAC_ADDR,IP);
	
	#define pgprintf(...) {sprintf(buffer,__VA_ARGS__); pos=tgPageAdd(buf, pos, buffer);}
	char buffer[512]; //buffer for pgprintf
	
	unsigned long hitcounter=0;
	
	unsigned char frequency=0;
	char *frequencynames[]={"Disabled","25MHz","12.5MHz","8.33MHz","6.25MHz","3.125MHz"};
	
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
		#define is_page(name) (strncmp(name, (char *) &buf[pos + 4], strlen(name))==0)
		#define BACKPAGE {pos=tgPageAdd(buf,0,HTTP_RESP_OK); pos=tgPageAdd(buf,pos,back_html);}
		
		hitcounter++;
		
		//if not authorized
		if(!is_valid_auth(buf,AUTH)) {
			if(is_page("/ ")) {
				//Start page with login button
				pos=tgPageAdd(buf,0,HTTP_RESP_OK);
				pos=tgPageAdd(buf,pos,smallmenu_html);
				pos=tgPageAdd(buf,pos,start_html);
				pos=tgPageAdd(buf,pos,HTML_FOOTER);
			} else {
				if(is_page("/login ")) {
					//Send the auth request
					pos=tgPageAdd(buf,0,HTTP_AUTHENTICATE);
				} else {
					//Show login required error and redirect to homepage
					pos=tgPageAdd(buf,0,HTTP_RESP_OK);
					pos=tgPageAdd(buf,pos,smallmenu_html);
					pos=tgPageAdd(buf,pos,loginrequired_html);
					pos=tgPageAdd(buf,pos,HTML_FOOTER);
				}
			}
			tgHttpReply(buf,pos);
			continue;
		}
		
		//Giant if statment, the elses are required so the last one does the 404
		if(is_page("/logout ")) {
			//Send the auth request to confuse browser into deleting the session
			pos=tgPageAdd(buf,0,HTTP_AUTHENTICATE);
			tgHttpReply(buf,pos);
			continue;
		}
		else if(is_page("/login ")) {
			BACKPAGE;
		}
		
		else if(is_page("/ ")) {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,menu_html);
			pos=tgPageAdd(buf,pos,index_html);
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		
		else if(is_page("/status ")) {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,menu_html);
			pgprintf(status_html,hitcounter,button_presses);
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		
		else if(is_page("/led ")) {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,menu_html);
			pos=tgPageAdd(buf,pos,led_html);
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		else if(is_page("/ledon ")) {
			set_bit(P_OUT(LED_P),LED);
			BACKPAGE;
		}
		else if(is_page("/ledoff ")) {
			clear_bit(P_OUT(LED_P),LED);
			BACKPAGE;
		}
		else if(is_page("/blinkon ")) {
			system_tick=*blinkled;
			BACKPAGE;
		}
		else if(is_page("/blinkoff ")) {
			system_tick=0;
			BACKPAGE;
		}
		
		else if(is_page("/freq ")) {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,menu_html);
			pgprintf(freq_html,frequencynames[frequency]);
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		else if(is_page("/frequp ")) {
			frequency++;
			frequency%=6;
			enc28j60clkout(frequency);
			BACKPAGE;
		}
		else if(is_page("/freqdown ")) {
			frequency--;
			frequency%=6;
			enc28j60clkout(frequency);
			BACKPAGE;
		}
		
		else if(is_page("/rev ")) {
			pos=tgPageAdd(buf,0,HTTP_RESP_OK);
			pos=tgPageAdd(buf,pos,menu_html);
			pgprintf(rev_html,tgGetRev());
			pos=tgPageAdd(buf,pos,HTML_FOOTER);
		}
		
		else {
			//404
			pos=tgPageAdd(buf,0,HTTP_RESP_NOT_FOUND);
			pos=tgPageAdd(buf,pos,HTML_RESP_NOT_FOUND);
		}
		
		tgHttpReply(buf,pos);
	}
}
void (*app_entry_point)(void)=*final;
