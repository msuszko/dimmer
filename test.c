#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "power.c"

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

int main(void) {
	power_init();
	printf("first_stop: %hhd, next_stop: %hhd\n", first_stop, next_stop);
	set_power(0, 254);
	set_power(1, 128);
	set_power(2, 127);
	printf("set_power(1, 128)\n");
	printf("first_stop: %hhd, next_stop: %hhd\n", first_stop, next_stop);
	printf("stop2pins[next_stop][2]: %s\n", byte_to_binary(stop2pins[next_stop][2]));
	zero_cross();
	printf("zero_cross()\n");
	printf("first_stop: %hhd, next_stop: %hhd\n", first_stop, next_stop);
	
	while (TCNT1 < OCR1A) {
		TCNT1 = OCR1A + 1;
		printf("timer alarm at: %hd, ms: %hhd, next: %hhd\n", OCR1A, off_midstop, next_stop);
		timer_alarm();
		printf("PORTD: %s\n", byte_to_binary(PORTD));
	}
	printf("FIN\n");
	return 0;
}
