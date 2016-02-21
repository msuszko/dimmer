
#include <avr/io.h>
#include "pins.h"
#include "serial.h"

char out_buff[OUT_BUFF_SIZE];
volatile unsigned int out_buff_in = 0, out_buff_out = 0;
char in_buff[IN_BUFF_SIZE];
volatile unsigned int in_buff_in = 0;
  
void on_buffer_empty(void)
{
  	PORTD |= _BV(RSSEND); /* enable RS485 */
  	if (out_buff_in != out_buff_out) {
  		UDR = out_buff[out_buff_out];
  		out_buff_out = (out_buff_out + 1) % OUT_BUFF_SIZE;
	} else {
		PORTD &= ~_BV(RSSEND); /* disable RS485 */
		UCSRB &= ~_BV(UDRIE); /* wyłącz przerwania pustego bufora nadawania */
	}
}

void receive_byte(uint8_t byte)
{
	if (byte == '\n' || byte == '\r') {
		send_byte('!');
		if (in_buff_in > IN_BUFF_SIZE) {
			send_byte('E');
		} else if (in_buff_in == 0) {
			send_byte('_');
		} else {
			process_command(in_buff, in_buff_in);
			send_byte('p');
		}
		in_buff_in = 0;
		send_byte('\r');
		send_byte('\n');
		return;
	} 
	send_byte('<');
	send_byte(byte);
	send_byte('>');
	if (in_buff_in > IN_BUFF_SIZE) {
		send_byte('$');
		return;
	}

	in_buff[in_buff_in] = byte;
	in_buff_in++;
	send_byte('\r');
	send_byte('\n');
}

int send_byte(uint8_t byte)
{
    /* add byte to the queue */
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
