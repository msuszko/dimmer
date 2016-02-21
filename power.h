
#define PIN_NUM 13

typedef struct
{
	uint8_t power;
	int8_t next;
} spoint;


void power_init(void);

void set_power(int8_t pin, uint8_t power);

void zero_cross(void);

void timer_alarm(void);
