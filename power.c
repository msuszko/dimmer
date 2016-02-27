
#ifdef NO_AVR_TEST
#include "test_compat.h"
#else
#include <avr/io.h>
#endif

#include "power.h"
#include "phase_control.h"

int8_t first_stop = -1;
int8_t next_stop = -1;
int8_t off_midstop = -1;
/* stop_idx -> power */
spoint stops[PIN_NUM];
/* pin_idx -> stop */
int8_t pin2stop[PIN_NUM];
/* stop_idx -> pin[] */
uint8_t stop2pins[PIN_NUM][3];
uint8_t pin_mask[3];

struct
{
	int8_t port_no;
	uint8_t mask;
} ports[] = {
	{2, _BV(PD5)}, /*  1   0 */
	{2, _BV(PD4)}, /*  2   1 */
	{2, _BV(PD6)}, /*  3   2 */
	{2, _BV(PD7)}, /*  4   3 */
	{0, _BV(PB1)}, /*  5   4 */
	{0, _BV(PB0)}, /*  6   5 */
	{0, _BV(PB2)}, /*  8   6 */
	{1, _BV(PC1)}, /*  9   7 */
	{1, _BV(PC0)}, /* 10   8 */
	{1, _BV(PC2)}, /* 11   9 */
	{1, _BV(PC3)}, /* 12  10 */
	{1, _BV(PC4)}, /* 13  11 */
	{1, _BV(PC5)}  /* 14  12 */
};


void set_pin(uint8_t reg[], int8_t pin)
{
	reg[ports[pin].port_no] |= _BV(ports[pin].mask);
}

void unset_pin(uint8_t reg[], int8_t pin)
{
	reg[ports[pin].port_no] &= ~_BV(ports[pin].mask);
}

void power_init(void)
{
	int8_t idx;

	for (idx=0; idx<PIN_NUM; idx++) {
		stops[idx].power = 0;
		stops[idx].next = -1;
		pin2stop[idx] = -1;
		stop2pins[idx][0] = 0;
		stop2pins[idx][1] = 0;
		stop2pins[idx][2] = 0;
		set_pin(pin_mask, idx);
	}
}


int8_t set_power(int8_t pin, uint8_t power)
{
	int8_t zero_idx, idx, stop = 0;
	uint8_t lesser_power = 0;

	if (pin < 0 || pin > PIN_NUM) {
		return -1;
	}
	if (pin2stop[pin] != -1) {
		/* remove pin from stop data */
		stop = pin2stop[pin];
		unset_pin(stop2pins[stop], pin);
		/* remove stop if empty */
		if (stop2pins[stop][0] == 0 && stop2pins[stop][1] == 0 && stop2pins[stop][2] == 0) {
			if (first_stop == stop) {
				first_stop = stops[stop].next;
			} else {
				for (idx=0; idx<PIN_NUM; idx++) {
					if (stops[idx].next == stop) {
						stops[idx].next = stops[stop].next;
						break;
					}
				}
			}
			stops[stop].power = 0;
			stops[stop].next = -1;
		}
		pin2stop[pin] = -1;
	}

	/* find empty stop or stop with the same power */
	for (idx=0; idx<PIN_NUM; idx++) {
		if (stops[idx].power == power) {
			stop = idx;
			break;
		} else if (stops[idx].power == 0) {
			stops[idx].power = power;
			stop = idx;
			if (lesser_power > 0) {
				stops[idx].next = stops[zero_idx].next;
				stops[zero_idx].next = stop;
			}
			break;
		} else if (stops[idx].power < power && stops[idx].power > lesser_power) {
			lesser_power = stops[idx].power;
			zero_idx = idx;
		}
	}
	if (first_stop == -1) {
		first_stop = idx;
	}
	pin2stop[pin] = stop;

	set_pin(stop2pins[stop], pin);
	return 0;
}

void zero_cross(void)
{
	TCNT1 = 0; /* reset timer - count from zero */
	if (first_stop == -1) {
		TIMSK = 0;
		return;
	}
	next_stop = first_stop;
	OCR1A = delays[stops[first_stop].power];
	TIMSK |= _BV(OCIE1A); /* allow CTC interrupt */
}

void timer_alarm(void)
{
	int8_t stop;

	/* execute off midstop */
	if (off_midstop != -1) {
		if (next_stop == -1) {
			/* this is last stop; zero all pins and disable timer */
			TIMSK = 0;
			PORTB = 0;
			PORTC = 0;
			PORTD &= ~(pin_mask[2]);
			next_stop = first_stop;
		} else {
			PORTB = stop2pins[off_midstop][0];
			PORTC = stop2pins[off_midstop][1];
			PORTD = (PORTD & ~(pin_mask[2])) | stop2pins[off_midstop][2];
			OCR1A = delays[stops[next_stop].power];
		}
		off_midstop = -1;
		return;
	}

	/* set the next stop */
	stop = next_stop;
	next_stop = stops[stop].next;
	/* set timer comparator to fire interrupt on next stop */
	if ((next_stop == -1) || ((stops[stop].power-2) < stops[next_stop].power)) {
		off_midstop = stop;
		OCR1A = delays[stops[stop].power-1];
	} else {
		OCR1A = delays[stops[next_stop].power];
	}
	PORTB = stop2pins[stop][0];
	PORTC = stop2pins[stop][1];
	PORTD = (PORTD & ~(pin_mask[2])) | stop2pins[stop][2];
}
