TARGET=blink
SOURCES=serial.cpp

ARDUINO_LIB=arduino.a
ARDUINO=/usr/share/arduino/hardware/arduino

ARDUINO_LIB_CXXSRC = \
	HardwareSerial.cpp \
	Print.cpp \
	Stream.cpp \
	WString.cpp \
	new.cpp \
	CDC.cpp

ARDUINO_LIB_CSRC = \
	wiring.c \
	wiring_analog.c \
	wiring_digital.c

ARDUINO_LIB_CXXOBJ=$(ARDUINO_LIB_CXXSRC:.cpp=.o)	
ARDUINO_LIB_COBJ=$(ARDUINO_LIB_CSRC:.c=.o)	
CXXOBJ=$(SOURCES:.cpp=.o)

CXX=avr-g++
CC=avr-gcc
OBJC=avr-objcopy
AR=avr-ar
MCU=atmega328p
CFLAGS=-I. -Wall -Os -DF_CPU=16000000UL -I$(ARDUINO)/cores/arduino -DARDUINO=105 -I$(ARDUINO)/variants/standard
LDFLAGS=

ISPPORT=/dev/ttyACM0
ISPDIR=/usr/share/arduino/hardware/tools
ISP=$(ISPDIR)/avrdude
ISPFLAGS=-c arduino -p $(MCU) -P $(ISPPORT) -b 115200 -C $(ISPDIR)/avrdude.conf 

all: $(TARGET)

vpath %.cpp $(ARDUINO)/cores/arduino
vpath %.c $(ARDUINO)/cores/arduino

$(ARDUINO_LIB): $(ARDUINO_LIB_COBJ) $(ARDUINO_LIB_CXXOBJ)
	$(AR) rcs $(ARDUINO_LIB) $(ARDUINO_LIB_COBJ) $(ARDUINO_LIB_CXXOBJ)

.cpp.o:
	@echo -e "\tCXX $<" 
	@$(CXX) -mmcu=$(MCU) -c -o $@ $< $(CFLAGS)
				
.c.o:
	@echo -e "\tCC $<"
	@$(CC) -mmcu=$(MCU) -c -o $@ $< $(CFLAGS)
	
$(TARGET): $(CXXOBJ) $(ARDUINO_LIB)
	@echo -e "\tLINKING" $<
	@$(CXX) -mmcu=$(MCU) -o $(TARGET) $(CXXOBJ) $(ARDUINO_LIB)
	$(OBJC) -O ihex -R .eeprom $(TARGET) $(TARGET).hex

clean:
	@echo ========== cleanup ========== 
	rm -rf *.o *.hex $(TARGET) $(ARDUINO_LIB)	

read:
	$(ISP) $(ISPFLAGS) -U flash:r:$(TARGET)_backup.hex:i

install:
	$(ISP) $(ISPFLAGS) -U flash:w:$(TARGET).hex
