#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h> 
#include <avr/eeprom.h>
#include <avr/interrupt.h> 

#define T1 PD5
#define T2 PD4
#define T3 PD6
#define T4 PD7
#define T5 PB1
#define T6 PB0
#define T7 -1
#define T8 PB2
#define T9 PC1
#define T10 PC0
#define T11 PC2
#define T12 PC3
#define T13 PC4
#define T14 PC5
#define ZERODET PD3
#define RSSEND PD2
#define TEMP ADC7

#define BAUD 9600
#include <util/setbaud.h>

#include "power.h"

uint8_t command;
uint8_t device_id __attribute__((section(".eeprom")));

#define BUFF_SIZE 32
char usart_buff[BUFF_SIZE];
volatile unsigned int usart_buff_in = 0, usart_buff_out = 0;

int send_byte(uint8_t byte);

void init(void)
{
	/* setup UART */
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE);
	UCSRC = _BV(URSEL)|_BV(UCSZ1)|_BV(UCSZ0); // 8N1
#if USE_2X  
	UCSRA |=  _BV(U2X);  
#else  
    UCSRA &= ~_BV(U2X);  
#endif  

	// setup analog input for temperature sensor
	ADCSRA |= _BV(ADEN)| // Analog-Digital enable bit
		      _BV(ADPS1)| // set prescaler to 8 (clock / 8)
		      _BV(ADPS2)| // set prescaler to 8 (clock / 8)
		      _BV(ADPS0); // set prescaler to 8 (clock / 8)
	ADMUX = 0x07; // use ADC7
	ADMUX |= _BV(REFS0)&~_BV(REFS1); //Avcc(+5v) as voltage reference

	// timer
	//TCCR1B |= _BV(WGM12); // CTC mode
	TCCR1B |= _BV(CS11); // prescaler to Fcpu/8
	OCR1A = 15625; // Ustawia wartość pożądaną na 1Hz dla preskalera 64
	TIMSK |= _BV(OCIE1A); // allow CTC interrupt

	// setup external interrupt for zero crossing detection
	DDRD &= ~_BV(DDD3);     // Clear the PD3 pin
	MCUCR |= _BV(ISC10)|_BV(ISC11); // trigger interrupt on rising INT1
	GICR |= _BV(INT1);      // Turns on INT1

	// setup output ports
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
	//PORTD = PORTD ^ _BV(T1);
	timer_alarm();
}

ISR(INT1_vect)
{
	zero_cross();
	// TCNT1 = 0;   //reset timer - count from zero
	// TCCR1B |= _BV(CS11); // prescaler to Fcpu/8

} 

// przerwanie generowane po odebraniu bajtu  
ISR(USART_RXC_vect)  
{  
	uint8_t u8Data;

	u8Data = UDR;
	switch (u8Data)
	{
		case 'v':
			send_byte(1);
		break;
		default:
			//send_byte(byte);
		break;
	}
}  

// przerwanie generowane, gdy bufor nadawania jest już pusty,   
ISR(USART_UDRE_vect){  
  
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

int read_temp(void)
{
	uint8_t temperature;

	ADCSRA |= _BV(ADSC);    // Start the ADC conversion
	temperature = ADCH;
	return temperature;
}

int main(void) 
{
    int i=0;

	power_init();
    init();    
    send_byte('X');
	for (;;) {
		i+=1;
   	} 
	return 0;
}
