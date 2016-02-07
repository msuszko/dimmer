
#include <avr/io.h>
#include "pins.h"
#include "serial.h"

char out_buff[OUT_BUFF_SIZE];
volatile unsigned int out_buff_in = 0, out_buff_out = 0;
char in_buff[IN_BUFF_SIZE];
volatile unsigned int in_buff_in = 0;
  
void on_buffer_empty(void)
{
  	UDR = 'X';
  	PORTD |= _BV(RSSEND); // enable RS485
  	if (out_buff_in != out_buff_out) {
  		UDR = out_buff[out_buff_out];
  		out_buff_out = (out_buff_out + 1) % OUT_BUFF_SIZE;
	} else {
		PORTD &= ~_BV(RSSEND); // disable RS485
		UCSRB &= ~_BV(UDRIE); //wyłącz przerwania pustego bufora nadawania
	}
}

void receive_byte(uint8_t byte)
{
	if (byte == 10 && byte == 13) {
		if (in_buff_in > IN_BUFF_SIZE) {
			send_byte('E');
			send_byte('\n');
		} else {
			process_command(in_buff, in_buff_in);
		}
		in_buff_in = 0;
		return;
	}
	if (in_buff_in > IN_BUFF_SIZE)
		return;

	in_buff[in_buff_in] = byte;
	in_buff_in++;
}

int send_byte(uint8_t byte)
{
    // add byte to the queue
	if (out_buff_in == ((out_buff_out - 1 + OUT_BUFF_SIZE) % OUT_BUFF_SIZE)) {
		return -1;
	} else {
		out_buff[out_buff_in] = byte;
		out_buff_in = (out_buff_in + 1) % OUT_BUFF_SIZE;
	}
	UCSRB |= _BV(UDRIE);
	return 0;
}

void respond(uint8_t byte)
{
	send_byte(byte);
	send_byte('\n');
}
