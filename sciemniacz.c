#define F_CPU 16000000UL 

#include <avr/io.h> 
#include <util/delay.h> 
#include <avr/interrupt.h> 
// #include "uart.h"

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

#define USART_BAUDRATE 19200
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

ISR(TIMER0_OVF_vect)
{
	PORTD = PORTD ^ (1 << T1);
}

int read_temp(void)
{
	uint8_t temperature;

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion
	temperature = ADCH;
	return temperature;
}

void init(void)
{
	/* setup external interrupt for xero crossing detection */
	DDRD &= ~(1 << DDD3);     // Clear the PD3 pin
	MCUCR |= (1 << ISC10)|(1 << ISC11); // trigger interrupt on rising INT1
	GICR |= (1 << INT1);      // Turns on INT1

	/* setup output ports */
	DDRB = (1 << DDB0)|(1 << DDB1)|(1 << DDB2);
	DDRC = (1 << DDC0)|(1 << DDC1)|(1 << DDC2)|(1 << DDC3)|(1 << DDC4)|(1 << DDC5);
	DDRD = (1 << DDD2)|(1 << DDD4)|(1 << DDD5)|(1 << DDD6)|(1 << DDD7);
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;

	/* setup UART */
	UBRRH = (uint8_t)(UBRR_VALUE >> 8); // baud rate
	UBRRL = (uint8_t)UBRR_VALUE; // baud rate
	UCSRC = (1 << URSEL)|(1 << UCSZ1)|(1 << UCSZ0); // 8N1
	UCSRB |= (1 << RXEN)|(1 << TXEN); // enable transmission and reception

	/* setup analog input for temperature sensor */
	ADCSRA |= (1 << ADEN)| // Analog-Digital enable bit
		      (1 << ADPS1)| // set prescaler to 8 (clock / 8)
		      (1 << ADPS2)| // set prescaler to 8 (clock / 8)
		      (1 << ADPS0); // set prescaler to 8 (clock / 8)
	ADMUX = 0x07; // use ADC7
	ADMUX |= (1 << REFS0)&~(1 << REFS1); //Avcc(+5v) as voltage reference

	/* timer */
	TIMSK=0x01;
	TCCR0=0x05;

	sei();
}

int main(void) 
{
	uint8_t u8Data;

    init();    
	for (;;) {
		/* receive */
		while(!(UCSRA&(1 << RXC))){};
		u8Data = UDR;
	
		u8Data++; // increment

		/* send */
		while(!(UCSRA&(1 << UDRE))){};
		UDR = 'u';

   	} 
	return 0;
}


