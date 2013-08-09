TARGET=marque
SOURCES=main.c marquee.c display.c font.c port.c
DEPS=
COBJ=$(SOURCES:.c=.o)

CC=avr-gcc
OBJC=avr-objcopy
MCU=atmega328p
CFLAGS=-I. -I../../pca/include/ -Wall -Os -DF_CPU=16000000UL -std=c99
LDFLAGS=

ISPPORT=/dev/ttyACM0
ISPDIR=/usr/share/arduino/hardware/tools
ISP=$(ISPDIR)/avrdude
ISPFLAGS=-c arduino -p $(MCU) -P $(ISPPORT) -b 115200 -C $(ISPDIR)/avrdude.conf 

all: $(TARGET)

%.o: %.c $(DEPS)
	@echo -e "\tCC" $<
	@$(CC) -mmcu=$(MCU) -c -o $@ $< $(CFLAGS)

$(TARGET): $(COBJ)
	@echo -e "\tLINKING CC" $<
	@$(CC) -mmcu=$(MCU) -o $(TARGET) $(COBJ)
	$(OBJC) -O ihex -R .eeprom $(TARGET) $(TARGET).hex

clean:
	@echo ========== cleanup ========== 
	rm -rf *.o *.hex $(TARGET)	

read:
	$(ISP) $(ISPFLAGS) -U flash:r:$(TARGET)_backup.hex:i

install:
	$(ISP) $(ISPFLAGS) -U flash:w:$(TARGET).hex
