/*******************************************************************************
*
* tgNetStack.h
*
* Convenience file for the tuxgraphics network stack.
*
*******************************************************************************/
#pragma once

#include <stdlib.h>

#include "ip_arp_udp_tcp.h"
#include "enc28j60.h"
#define HTTP_PORT 80

#define tgNetStackInit(macAddr, ipAddr) { \
	enc28j60Init(macAddr); \
	enc28j60clkout(0); \
	__delay_cycles(500); \
	enc28j60PhyWrite(PHLCON, 0x476); \
	init_udp_or_www_server(macAddr, ipAddr); \
	www_server_port(HTTP_PORT); \
}

const char *HTTP_RESP_OK = "HTTP/1.0 200 OK\nContent­Type: text/html\nPragma: no­cache\n\n";
//const char *HTTP_RESP_UNAUTH = "HTTP/1.0 401 UA\n\n";
const char *HTTP_RESP_NOT_FOUND = "HTTP/1.0 404 NF\n\n";
const char *HTTP_RESP_REDIRECT = "HTTP/1.0 302 Found\nLocation: /";
const char *HTTP_AUTHENTICATE = "HTTP/1.0 401 Access Denied\r\nWWW-Authenticate: Basic\r\nContent-Length: 0\r\n\r\n";


const char *HTML_RESP_OK = "200 OK\n\n";
const char *HTML_RESP_UNAUTH = "401 Unauthorized Access\n\n";
const char *HTML_RESP_NOT_FOUND = "404 Page Not Found\n\n";

const char *HTML_REFRESH_HEADER = "<!DOCTYPE html>"
"<head>"
"<meta http-equiv=\"refresh\" content=\"1\">"
"</head>"
"<body>";
const char *HTML_FOOTER="</body></html>";

#define tgHttpReply(a,b) www_server_reply(a,b)
#define tgPageAdd(a,b,c) fill_tcp_data(a,b,c)
#define tgPacketGet(a,b) packetloop_arp_icmp_tcp(a, enc28j60PacketReceive(b, a))