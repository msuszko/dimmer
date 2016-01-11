
#include <avr/io.h>
#include "power.h"
#include "phase_control.h"

int8_t first_stop = -1;
int8_t next_stop = -1;
// stop_idx -> power
spoint stops[PIN_NUM];
// pin_idx -> stop
int8_t pin2stop[PIN_NUM];
// stop_idx -> pin[]
int8_t stop2pins[PIN_NUM][PIN_NUM];

void power_init(void)
{
	int8_t idx, sidx;

	for (idx=0; idx<PIN_NUM; idx++) {
		stops[idx].power = 0;
		stops[idx].next = -1;
		pin2stop[idx] = -1;
		for (sidx=0; sidx<PIN_NUM; sidx++) {
			stop2pins[idx][sidx] = -1;
		}
	}
}


void set_power(int8_t pin, uint8_t power)
{
	int8_t idx, prev_idx, stop = 0;
	int8_t zero_idx = 0;
	uint8_t lesser_power = 0;

	if (pin2stop[pin] != -1) {
		// remove pin from stop data
		stop = pin2stop[pin];
		// stop_pins = stop2pins[stop];
		zero_idx = 0;
		for (idx=0; idx<PIN_NUM; idx++) {
			if (stop2pins[stop][idx] == pin) {
				stop2pins[stop][idx] = -1;
				zero_idx = idx;
			} else if (stop2pins[stop][idx] == -1) {
				prev_idx = idx - 1;
				if (prev_idx > zero_idx) {
					// move last value to empty slot
					stop2pins[stop][zero_idx] = stop2pins[stop][prev_idx];
					stop2pins[stop][prev_idx] = -1;
				}
				break;
			}
		}
		// remove stop if empty
		if (zero_idx == 0) {
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

	// find empty stop or stop with the same power
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
	pin2stop[pin] = stop;

	// add pin to stop data at first empty slot
	for (idx=0; idx<PIN_NUM; idx++) {
		if (stop2pins[stop][idx] == 0) {
			stop2pins[stop][idx] = pin;
			break;
		}
	}
}

void zero_cross(void)
{
	TCNT1 = 0;   //reset timer - count from zero
	if (first_stop == -1) {
		TIMSK = 0;
		return;
	}
	next_stop = stops[first_stop].next;
	OCR1A = delays[stops[first_stop].power];
	TIMSK |= _BV(OCIE1A); // allow CTC interrupt
}

void timer_alarm(void)
{
	int8_t idx, stop, pin;

	if (stops[next_stop].next == -1) {
		next_stop = first_stop;
		TIMSK = 0;
	} else {
		stop = next_stop;
		next_stop = stops[stop].next;
		OCR1A = delays[stops[stop].power];
	}
	for (idx=0; idx<PIN_NUM; idx++) {
		pin = stop2pins[stop][idx];
		if (pin == -1) {
			break;
		} else {
			// enable
		}
	}
}
