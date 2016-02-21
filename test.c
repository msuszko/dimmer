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
	printf("first_stop: %hhd\n", first_stop);
	set_power(1, 128);
	set_power(2, 18);
	printf("first_stop: %hhd\n", first_stop);
	zero_cross();
	
	printf("timer alarm at: %hd\n", OCR1A);
	timer_alarm();
	printf("PORTD: %s\n", byte_to_binary(PORTD));

	printf("timer alarm at: %hd\n", OCR1A);
	timer_alarm();
	printf("PORTD: %s\n", byte_to_binary(PORTD));

	printf("plunk\n");
	return 0;
}
