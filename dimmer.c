
#include <avr/io.h>
#include <util/delay.h> 
#include <avr/eeprom.h>
#include <avr/interrupt.h> 

#include "pins.h"
#include "serial.h"
#include "power.h"

uint8_t command;
uint8_t device_id __attribute__((section(".eeprom")));


void init(void)
{
	/* setup UART */
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE);
	UCSRC = _BV(URSEL)|_BV(UCSZ1)|_BV(UCSZ0); /* 8N1 */
#if USE_2X  
	UCSRA |=  _BV(U2X);  
#else  
    UCSRA &= ~_BV(U2X);  
#endif  

	/* setup analog input for temperature sensor */
	ADCSRA |= _BV(ADEN)| /* Analog-Digital enable bit */
		      _BV(ADPS1)| /* set prescaler to 8 (clock / 8) */
		      _BV(ADPS2)| /* set prescaler to 8 (clock / 8) */
		      _BV(ADPS0); /* set prescaler to 8 (clock / 8) */
	ADMUX = 0x07; /* use ADC7 */
	ADMUX |= _BV(REFS0)&~_BV(REFS1); /* Avcc(+5v) as voltage reference */

	/* timer */
	/* TCCR1B |= _BV(WGM12); / * CTC mode */
	TCCR1B |= _BV(CS11); /* prescaler to Fcpu/8 */
	OCR1A = 15625; /* Ustawia wartość pożądaną na 1Hz dla preskalera 64 */
	TIMSK |= _BV(OCIE1A); /* allow CTC interrupt */

	/* setup external interrupt for zero crossing detection */
	DDRD &= ~_BV(DDD3);     /* Clear the PD3 pin */
	MCUCR |= _BV(ISC10)|_BV(ISC11); /* trigger interrupt on rising INT1 */
	GICR |= _BV(INT1);      /* Turns on INT1 */

	/* setup output ports */
	DDRB = _BV(DDB0)|_BV(DDB1)|_BV(DDB2);
	DDRC = _BV(DDC0)|_BV(DDC1)|_BV(DDC2)|_BV(DDC3)|_BV(DDC4)|_BV(DDC5);
	DDRD = _BV(DDD2)|_BV(DDD4)|_BV(DDD5)|_BV(DDD6)|_BV(DDD7);
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;

	sei();
}

ISR(TIMER1_COMPA_vect)
{
	/* PORTD = PORTD ^ _BV(T1); */
	timer_alarm();
}

ISR(INT1_vect)
{
	zero_cross();
	/* TCNT1 = 0;   / * reset timer - count from zero */
	/* TCCR1B |= _BV(CS11); / * prescaler to Fcpu/8 */

} 

/* przerwanie generowane po odebraniu bajtu   */
ISR(USART_RXC_vect)  
{  
	uint8_t u8Data;

	u8Data = UDR;
	receive_byte(u8Data);
}  

/* przerwanie generowane, gdy bufor nadawania jest już pusty */
ISR(USART_UDRE_vect){  
	on_buffer_empty();
}  

int read_temp(void)
{
	uint8_t temperature;

	ADCSRA |= _BV(ADSC); /* Start the ADC conversion */
	temperature = ADCH;
	return temperature;
}

void process_command(char *cmd_buff, unsigned int len)
{
	switch (cmd_buff[0]) {
		case 'o':
			if (len != 3) {
				respond('l');
				return;
			}
			set_power((int8_t)cmd_buff[1], (uint8_t)cmd_buff[2]);
			respond('o');
		break;
		case 'S':
			/* TODO: set id in eeprom */
			respond('S');
		break;
		case 'i':
			/* TODO: read id from eeprom */
			send_byte('i');
			send_byte(0);
			send_byte(1);
			send_byte('\n');
		break;
		case 't':
			/* TODO: read temperature */
			send_byte('t');
			send_byte(100);
			send_byte('\n');
		break;
		default:
			respond('u');
		break;
	}
}

int main(void) 
{
    int i=0;

	power_init();
    init();    
    send_byte('r');
    send_byte('\n');
	for (;;) {
		i+=1;
   	} 
	return 0;
}
