TARGET=dimmer

CC=avr-gcc

# PORT=/dev/cuaU0
PORT=avrdoper
PROGRAMMER=stk500v2

MCU=atmega8
INC=

CFLAGS=-g -mmcu=$(MCU) -I. $(INC)
CFLAGS+= -pedantic-errors -Werror -Wall
CFLAGS+= -flto -mrelax
CFLAGS+= -DF_CPU=16000000UL


# opt
CFLAGS+= -Os
#CFLAGS+= -fdata-sections -ffunction-sections
#CFLAGS+= -mcall-prologues
#CFLAGS+= -funsigned-char
#CFLAGS+= -funsigned-bitfields
#CFLAGS+= -fpack-struct
#CFLAGS+= -fshort-enums 
#CFLAGS+= -finline-limit=3
#CFLAGS+= -fno-inline-small-functions 
# end opt

LDFLAGS=-g -lc -mmcu=$(MCU) -Wl,-Map,$(TARGET).map
LFDLAGS+= -flto -Wl,--relax
LDFLAGS+= -Wl,--gc-sections -Wl,--print-gc-sections


.SUFFIXES: .elf .bin .c .h .o
src= power.c serial.c $(TARGET).c
objs = $(src:.c=.o)

.c.o:
	$(CC) $(CFLAGS) -c ${.IMPSRC} -o ${.TARGET}

$(TARGET).elf: $(objs)
	$(CC) $(LDFLAGS) -o ${.TARGET} ${.ALLSRC}
	avr-size --format=berkeley $(TARGET).elf
	#avr-size --format=avr --mcu=$(MCU) $(TARGET).elf

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -j .text -j .data -O ihex ${.ALLSRC} ${.TARGET}

$(TARGET).bin: $(TARGET).bin
	avr-objcopy -j .text -j .data -O binary ${.ALLSRC} ${.TARGET}

flash: $(TARGET).hex
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U flash:w:$(TARGET).hex:i

fuse:
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U lfuse:w:0xff:m -U hfuse:w:0xc9:m 

clean:
	rm -f *.elf *.o *.bin *.map

test_bin: power.c test.c power.h test_compat.h
	cc -DNO_AVR_TEST -o test_bin test.c

all: $(TARGET).bin


.PHONY: clean fuse
