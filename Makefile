TARGET=dimmer

CC=avr-gcc
SIZE=avr-size

# PORT=/dev/cuaU0
PORT=avrdoper
PROGRAMMER=stk500v2

MCU=atmega8
INC=

CFLAGS=-g -mmcu=$(MCU) -Wall -I. $(INC)
CFLAGS+= -pedantic-errors -Werror -mcall-prologues
CFLAGS+= -flto
CFLAGS+= -DF_CPU=16000000UL


# opt
CFLAGS += -Os
#CFLAGS += -fdata-sections -ffunction-sections
#CFLAGS += -funsigned-char
#CFLAGS += -funsigned-bitfields
#CFLAGS += -fpack-struct
#CFLAGS += -fshort-enums 
#CFLAGS += -finline-limit=3
#CFLAGS += -fno-inline-small-functions 
# end opt

LDFLAGS =-g -lc -mmcu=$(MCU) -Wl,-Map,$(TARGET).map
LFDLAGS=-flto
#LDFLAGS += -Wl,--gc-sections -Wl,--print-gc-sections
#LDFLAGS += -Wl,--relax


.SUFFIXES: .elf .bin .c .h .o
CSRC = power.c serial.c $(TARGET).c
OBJS= $(TARGET).o power.o serial.o

.c.o:
	$(CC) $(CFLAGS) -c ${.IMPSRC} -o ${.TARGET}

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o ${.TARGET} ${.ALLSRC}

$(TARGET).bin: $(TARGET).elf
	avr-objcopy -O binary ${.ALLSRC} ${.TARGET}
	$(SIZE) --format=berkeley $(TARGET).elf
	$(SIZE) --format=avr --mcu=$(MCU) $(TARGET).elf
	#avr-objcopy -j .text -j .data -O binary ${.ALLSRC} ${.TARGET}

all: $(TARGET).bin

flash: $(TARGET).bin
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U flash:w:$(TARGET).bin:r

fuse:
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U lfuse:w:0xff:m -U hfuse:w:0xc9:m 

clean:
	rm -f *.elf *.o *.bin *.map


.PHONY: clean fuse
