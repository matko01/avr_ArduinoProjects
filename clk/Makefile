TARGET=clk
SOURCES= \
		 sys_common.c \
		 interrupts.c \
		 temperature.c \
		 lcd.c \
		 main.c

DEPS=
COBJ=$(SOURCES:.c=.o)
ASRC=$(SOURCES:.c=.s)
PCA_PREFIX=../../pca

CC=avr-gcc
AS=avr-as
STRIP=avr-strip
OBJC=avr-objcopy
MCU=atmega328p
CFLAGS=-I. -I$(PCA_PREFIX)/include/ -Wall -Os -DF_CPU=16000000UL -std=gnu99 \
	   -ffunction-sections \
	   -fdata-sections \
	   -ffreestanding \
	   -fno-tree-scev-cprop \
	   -fpack-struct
LDFLAGS=-lpca -L$(PCA_PREFIX) -Wl,--gc-sections,--relax

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

$(TARGET): $(COBJ) libpca.a
	@echo -e "\tLINKING CC" $<
	@$(CC) -mmcu=$(MCU) -o $(TARGET) $(COBJ) $(LDFLAGS)
	
	$(OBJC) -O ihex -R .eeprom $(TARGET) $(TARGET).hex
	$(OBJC) -O binary -R .eeprom $(TARGET) $(TARGET).bin

clean:
	$(MAKE) -C $(PCA_PREFIX) clean
	@echo ========== cleanup ========== 
	rm -rf *.s *.o *.bin *.hex a.out $(TARGET)	

read:
	$(ISP) $(ISPFLAGS) -U flash:r:$(TARGET)_backup.hex:i

install:
	$(ISP) $(ISPFLAGS) -U flash:w:$(TARGET).hex
