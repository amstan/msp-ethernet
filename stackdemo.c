#include "hardware.h"
#include "spi.h"

#include <stdlib.h>
#include <string.h>
#include "stack/ip_arp_udp_tcp.h"
#include "stack/enc28j60.h"

// please modify the following two lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
// how did I get the mac addr? Translate the first 3 numbers into ascii is: TUX
static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x29};
static uint8_t myip[4] = {192,168,0,2}; // aka http://10.0.0.29/

// server listen port for www
#define MYWWWPORT 80

// global packet buffer
#define BUFFER_SIZE 550
static uint8_t buf[BUFFER_SIZE+1];

uint16_t http200ok(void)
{
	return(fill_tcp_data(buf,0,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
}

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage(uint8_t *buf)
{
	uint16_t plen;
	plen=http200ok();
	plen=fill_tcp_data(buf,plen,"<pre>");
	plen=fill_tcp_data(buf,plen,"Hi!\nYour web server works great.");
	plen=fill_tcp_data(buf,plen,"</pre>\n");
	return(plen);
}

void stackdemo(void) {
	
	clear_bit(P_OUT(LED_P),LED);
	
	//Prepare spi for the ethernet chip
	spi_init(UCCKPL*0+UCCKPH*1); //Clock idle in a low state, data on rising edge
	
	uint16_t dat_p;
	
	//initialize the hardware driver for the enc28j60
	enc28j60Init(mymac);
	enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
	__delay_cycles(160*6); // 60us
	enc28j60PhyWrite(PHLCON,0x476);
	
	//init the ethernet/ip layer:
	init_udp_or_www_server(mymac,myip);
	www_server_port(MYWWWPORT);

	while(1) {
		// read packet, handle ping and wait for a tcp packet:
		dat_p=packetloop_arp_icmp_tcp(buf,enc28j60PacketReceive(BUFFER_SIZE, buf));
		set_bit(P_OUT(LED_P),LED);
		
		// dat_p will be unequal to zero if there is a valid  http get
		if(dat_p==0){
			// no http request
			continue;
		}
		// tcp port 80 begin
		if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
			// head, post and other methods:
			dat_p=http200ok();
			dat_p=fill_tcp_data(buf,dat_p,"<h1>200 OK</h1>");
			goto SENDTCP;
		}
		// just one web page in the "root directory" of the web server
		if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0){
			dat_p=print_webpage(buf);
			goto SENDTCP;
		}else{
			dat_p=fill_tcp_data(buf,0,"HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>");
			goto SENDTCP;
		}
SENDTCP:
		www_server_reply(buf,dat_p); // send web page data
		// tcp port 80 end
	}
}
void (*app_entry_point)(void)=*stackdemo;

void tick(void) {
	//toggle_bit(P_OUT(LED_P),LED);
}
void (*system_tick)(void)=*tick;