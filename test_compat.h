
#define uint8_t unsigned char
#define int8_t char

unsigned short TCNT1, OCR1A = 0;
uint8_t PORTB = 0,
		PORTC = 0,
		PORTD = 0,
		TIMSK = 0;

#define _BV(bit) (1 << (bit))

#define OCIE1A 1

#define PB0 0
#define PB1 1
#define PB2 2
#define PB3 3
#define PB4 4
#define PB5 5
#define PB6 6
#define PB7 7
#define PC0 0
#define PC1 1
#define PC2 2
#define PC3 3
#define PC4 4
#define PC5 5
#define PC6 6
#define PC7 7
#define PD0 0
#define PD1 1
#define PD2 2
#define PD3 3
#define PD4 4
#define PD5 5
#define PD6 6
#define PD7 7
