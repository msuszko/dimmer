#define F_CPU 16000000UL

#include <avr/io.h> 
#include <util/delay.h> 
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

uint8_t stop_ptr;
uint16_t stops[13];

#define BUFF_SIZE 32
char usart_buff[BUFF_SIZE];
volatile unsigned int usart_buff_in = 0, usart_buff_out = 0;


int send_byte(uint8_t byte);

void init(void)
{
	/* setup UART */
	UCSRB = (1 << TXEN)|(1 << RXEN)|(1 << RXCIE);
	UCSRC = (1 << URSEL)|(1 << UCSZ1)|(1 << UCSZ0); // 8N1
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X  
	UCSRA |=  (1<<U2X);  
#else  
    UCSRA &= ~(1<<U2X);  
#endif  

	// setup analog input for temperature sensor
	ADCSRA |= (1 << ADEN)| // Analog-Digital enable bit
		      (1 << ADPS1)| // set prescaler to 8 (clock / 8)
		      (1 << ADPS2)| // set prescaler to 8 (clock / 8)
		      (1 << ADPS0); // set prescaler to 8 (clock / 8)
	ADMUX = 0x07; // use ADC7
	ADMUX |= (1 << REFS0)&~(1 << REFS1); //Avcc(+5v) as voltage reference

	// timer
	//TCCR1B |= (1 << WGM12); // CTC mode
	//TCCR1B |= ((1 << CS10) | (1 << CS11)); // prescaler to Fcpu/64
	TCCR1B = 0x04; //start timer with prescaler Fcpu/256
	OCR1A = 15625; // Ustawia wartość pożądaną na 1Hz dla preskalera 64
	TIMSK |= (1 << OCIE1A); // allow CTC interrupt

	// setup external interrupt for zero crossing detection
	DDRD &= ~(1 << DDD3);     // Clear the PD3 pin
	MCUCR |= (1 << ISC10)|(1 << ISC11); // trigger interrupt on rising INT1
	GICR |= (1 << INT1);      // Turns on INT1

	// setup output ports
	DDRB = (1 << DDB0)|(1 << DDB1)|(1 << DDB2);
	DDRC = (1 << DDC0)|(1 << DDC1)|(1 << DDC2)|(1 << DDC3)|(1 << DDC4)|(1 << DDC5);
	DDRD = (1 << DDD2)|(1 << DDD4)|(1 << DDD5)|(1 << DDD6)|(1 << DDD7);
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;

	sei();
}

ISR(TIMER1_COMPA_vect)
{
	PORTD = PORTD ^ (1 << T1);
}

ISR(INT1_vect)
{
	TCCR1B = 0x04; //start timer with prescaler Fcpu/256
	TCNT1 = 0;   //reset timer - count from zero
} 

// przerwanie generowane po odebraniu bajtu  
ISR(USART_RXC_vect)  
{  
	uint8_t u8Data;

	u8Data = UDR;
	u8Data++; // increment
	send_byte(u8Data);
    
}  

// przerwanie generowane, gdy bufor nadawania jest już pusty,   
ISR(USART_UDRE_vect){  
  
  	PORTD |= (1<<RSSEND); // enable RS485
  	if (usart_buff_in != usart_buff_out) {
  		UDR = usart_buff[usart_buff_out];
  		usart_buff_out = (usart_buff_out + 1) % BUFF_SIZE;
	} else {
		PORTD &= ~(1<<RSSEND); // disable RS485
		UCSRB &= ~(1<<UDRIE); //wyłącz przerwania pustego bufora nadawania
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
	UCSRB |= (1<<UDRIE);
	return 0;
}

int read_temp(void)
{
	uint8_t temperature;

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion
	temperature = ADCH;
	return temperature;
}

int main(void) 
{
    int i=0;

    init();    
    send_byte('X');
	for (;;) {
		i+=1;
   	} 
	return 0;
}


