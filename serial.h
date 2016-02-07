
#define BAUD 9600
#include <util/setbaud.h>

#define OUT_BUFF_SIZE 32
#define IN_BUFF_SIZE 32

void on_buffer_empty(void);
void receive_byte(uint8_t byte);
int send_byte(uint8_t byte);
void respond(uint8_t byte);
void process_command(char *cmd_buff, unsigned int len);
