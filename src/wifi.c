
#include "wifi.h"
#include "string.h"
uint8_t idle_count = 0;
uint8_t crc = 0;
uint8_t answer_cmd[50] = {0x55, 0xAA, 0x03};
uint8_t answer_cmd1[50] = {0x55, 0xAA, 0x03};
char prod_info[] = "{\"p\":\"helloworld!!!!!!\",\"v\":\"1.1.1\",\"m\":0}";

uint8_t chksum8(const uint8_t *buff, size_t len)
{
    uint8_t sum;       
    for ( sum = 0 ; len != 0 ; len-- )
        sum += *(buff++); 
		
    return sum;
}

void usart_transmit_frame(const uint8_t *buff, size_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		usart_data_transmit(USART1, buff[i]);
		while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
	}
}

void receive_uart_int()
{
	static uint8_t i = 0;
  static uint8_t recv_counter = 0;
	if(idle_flag_stat==0)
	{
		recv_buffer[i++] = usart_data_receive(USART1);
	}
	if(idle_flag_stat)
	{
		uint8_t j = 0;
		
		for(;recv_counter <= 255;recv_counter++)
		{
			if(j == i) break;
			recv_buffer_compl[recv_counter] = recv_buffer[j++];
		}
		//str_len_info = strlen(prod_info);
		
		// PARSER
		uint8_t pointer = 0;
		uint16_t payload_len = 0;
		uint8_t frame_cmd = 0;
		while(pointer != recv_counter)
		{
			if(recv_buffer_compl[pointer] == HEADER_1B && recv_buffer_compl[pointer+1] == HEADER_2B) // FIND HEADER 55AA
			{
				//pointer += HEADER_LEN;
				//pointer++;
				frame_cmd = recv_buffer_compl[pointer+3];
				payload_len = ((recv_buffer_compl[pointer+4] << 8) | recv_buffer_compl[pointer+5])  ;
				
				uint8_t *frame = new uint8_t[payload_len + 7];
				
				for(uint8_t ii = 0; ii < (payload_len + 7); ii++)
				{
					frame[ii] = recv_buffer_compl[pointer + ii];								
				}
				
				crc = chksum8(frame, payload_len + 6);
				
				if(crc == frame[payload_len + 6])
				{
					if(frame_cmd == CMD_HB)
					{
						answer_cmd[3] = CMD_HB;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x01;
						answer_cmd[6] = 0x01;
						answer_cmd[7] = chksum8(answer_cmd, 7);
						usart_transmit_frame(answer_cmd, 8);
						
						
					}
					if(frame_cmd == CMD_INFO)
					{
						answer_cmd[3] = CMD_INFO;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x2A;
						memmove(answer_cmd+6, prod_info, 0x2A);
						//answer_cmd[6] = 0x01;
						answer_cmd[6+0x2A] = chksum8(answer_cmd, 0x2A+6);
						usart_transmit_frame(answer_cmd, 7+0x2A);					
					}
					if(frame_cmd == CMD_WMODE)
					{
						answer_cmd[3] = CMD_WMODE;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x00;
						//memmove(answer_cmd+6, prod_info, 0x2A);
						//answer_cmd[6] = 0x00;
						answer_cmd[6] = chksum8(answer_cmd, 6);
						usart_transmit_frame(answer_cmd, 7);
					}			
					if(frame_cmd == CMD_WMODE)
					{
						answer_cmd[3] = CMD_WF_STAT;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x00;
						//memmove(answer_cmd+6, prod_info, 0x2A);
						//answer_cmd[6] = 0x00;
						answer_cmd[6] = chksum8(answer_cmd, 6);
						usart_transmit_frame(answer_cmd, 7);
						/*
						answer_cmd[3] = CMD_NET_CONF;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x01;
						//memmove(answer_cmd+6, prod_info, 0x2A);
						answer_cmd[6] = 0x00;
						answer_cmd[7] = chksum8(answer_cmd, 7);
						usart_transmit_frame(answer_cmd, 8);		

						answer_cmd[3] = CMD_RESET;
						answer_cmd[4] = 0x00;
						answer_cmd[5] = 0x00;
						//memmove(answer_cmd+6, prod_info, 0x2A);
						//answer_cmd[6] = 0x00;
						answer_cmd[6] = chksum8(answer_cmd, 6);
						usart_transmit_frame(answer_cmd, 7);*/					
					}						
				}
				
				
				
				delete []frame;	
			}
			
			pointer++;
		}
		//--------------
		recv_counter = 0;
		i = 0;
		idle_flag_stat = 0;
	}
}

