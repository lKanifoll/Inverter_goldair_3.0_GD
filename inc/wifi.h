#ifndef __WIFI_H
#define __WIFI_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "main.h"
	
#define HEADER_1B			0x55
#define HEADER_2B			0xAA
#define HEADER_LEN		0x03 // 55 AA [00] - 00 indicates frame from module.
#define CMD_HB				0x00
#define CMD_INFO			0x01	
#define CMD_WMODE			0x02		
#define CMD_WF_STAT		0x03		
#define CMD_RESET		  0x04
#define CMD_NET_CONF	0x05	
	
#define CMD_INPUT   	0x06
#define CMD_OUTPUT  	0x07	
	
#define CMD_QUERY			0x08
	
#define ID_SWITCH			0x01
#define ID_WORKMODE		0x04
#define ID_CURPOWER		0x0C
#define ID_COMFORT		0x65


uint8_t recv_buffer_compl[255];
uint8_t chksum8(const uint8_t *buff, size_t len);
//void receive_uart_int(void);
void usart_transmit_frame(const uint8_t *buff, size_t len);
	
	
	
	
	
#ifdef __cplusplus
}
#endif

#endif /* __WIFI_H */
