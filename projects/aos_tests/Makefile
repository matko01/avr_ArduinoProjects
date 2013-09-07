TARGET=sched_test
SOURCES=main.c

DEPS=
LIBS=libaos.a libpca.a
COBJ=$(SOURCES:.c=.o)
PCA_PREFIX=../../pca
SCHED_PREFIX=../../aos

CC=avr-gcc
OBJC=avr-objcopy
MCU=atmega328p
CFLAGS=-I. -I$(PCA_PREFIX)/include/ -I$(SCHED_PREFIX) -Wall -Os -DF_CPU=16000000UL -std=c99 
LDFLAGS=-laos -lpca -L$(SCHED_PREFIX) -L$(PCA_PREFIX) 

ISPPORT=/dev/ttyACM0
ISPDIR=/usr/share/arduino/hardware/tools
ISP=$(ISPDIR)/avrdude
ISPFLAGS=-c arduino -p $(MCU) -P $(ISPPORT) -b 115200 -C $(ISPDIR)/avrdude.conf 

all: $(TARGET)

%.o: %.c $(DEPS)
	@echo -e "\tCC" $<
	@$(CC) -mmcu=$(MCU) -c -o $@ $< $(CFLAGS)

libpca.a:
	@echo -e "\tBUILDING PURE C ARDUINO LIB"
	$(MAKE) -C $(PCA_PREFIX)

libaos.a:
	@echo -e "\tBUILDING AOS LIB"
	$(MAKE) -C $(SCHED_PREFIX)

$(TARGET): $(COBJ) $(LIBS)
	@echo -e "\tLINKING CC" $<
	@$(CC) -mmcu=$(MCU) -o $(TARGET) $(COBJ) $(LDFLAGS)
	$(OBJC) -O ihex -R .eeprom $(TARGET) $(TARGET).hex
	$(OBJC) -O binary -R .eeprom $(TARGET) $(TARGET).bin

clean:
	$(MAKE) -C $(SCHED_PREFIX) clean
	$(MAKE) -C $(PCA_PREFIX) clean
	@echo ========== cleanup ========== 
	rm -rf *.o *.bin *.hex $(TARGET)	

read:
	$(ISP) $(ISPFLAGS) -U flash:r:$(TARGET)_backup.hex:i

install:
	$(ISP) $(ISPFLAGS) -U flash:w:$(TARGET).hex
