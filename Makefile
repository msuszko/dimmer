TARGET=sciemniacz

CC=avr-gcc
# PORT=/dev/cuaU0
PORT=avrdoper
PROGRAMMER=stk500v2

MCU=atmega8
INC=

CFLAGS=-mmcu=$(MCU) -Wall -I. $(INC) -Os


.SUFFIXES: .elf .bin .c .o
OBJS= $(TARGET).o

.c.o:
	$(CC) $(CFLAGS) -c ${.IMPSRC} -o ${.TARGET}

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -Wl,-Map,$(TARGET).map -o ${.TARGET} ${.ALLSRC}

$(TARGET).bin: $(TARGET).elf
	avr-objcopy -j .text -j .data -O binary ${.ALLSRC} ${.TARGET}


all: $(TARGET).bin

flash: $(TARGET).bin
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U flash:w:$(TARGET).bin:r

fuse:
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U lfuse:w:0xff:m -U hfuse:w:0xc9:m 

clean:
	rm -f *.elf *.o *.bin *.map


.PHONY: clean fuse
