
#include <avr/io.h>
#include "pins.h"
#include "serial.h"

char usart_buff[BUFF_SIZE];
volatile unsigned int usart_buff_in = 0, usart_buff_out = 0;
  
void on_buffer_empty(void)
{
  	UDR = 'X';
  	PORTD |= _BV(RSSEND); // enable RS485
  	if (usart_buff_in != usart_buff_out) {
  		UDR = usart_buff[usart_buff_out];
  		usart_buff_out = (usart_buff_out + 1) % BUFF_SIZE;
	} else {
		PORTD &= ~_BV(RSSEND); // disable RS485
		UCSRB &= ~_BV(UDRIE); //wyłącz przerwania pustego bufora nadawania
	}
}

void receive_byte(uint8_t byte)
{
	switch (byte)
	{
		case 'v':
			send_byte(1);
		break;
		default:
			//send_byte(byte);
		break;
	}
}

int send_byte(uint8_t byte)
{
    // add byte to the queue
	if (usart_buff_in == ((usart_buff_out - 1 + BUFF_SIZE) % BUFF_SIZE)) {
		return -1;
	} else {
		usart_buff[usart_buff_in] = byte;
		usart_buff_in = (usart_buff_in + 1) % BUFF_SIZE;
	}
	UCSRB |= _BV(UDRIE);
	return 0;
}

