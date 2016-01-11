
#define PIN_NUM 13

// #define int8_t char
// #define uint8_t unsigned char

typedef struct
{
	uint8_t power;
	int8_t next;
} spoint;


void power_init(void);

void set_power(int8_t pin, uint8_t power);

void zero_cross(void);

void timer_alarm(void);
