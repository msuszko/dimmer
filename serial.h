
#define BAUD 9600
#include <util/setbaud.h>

#define BUFF_SIZE 32

void on_buffer_empty(void);
void receive_byte(uint8_t byte);
int send_byte(uint8_t byte);
