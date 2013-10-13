TARGET=host_slip
SOURCES= \
		 main.c \
		 common.c \
		 crc.c \
		 ../../../pca/src/slip.c

CC=gcc
COBJ=$(SOURCES:.c=.o)
CFLAGS=-I. -Wall -I../../../pca/include/
LDFLAGS=

all: $(TARGET)

%.o: %.c $(DEPS)
	@echo -e "\tCC" $<
	@$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(COBJ) 
	@echo -e "\tLINKING CC" $<
	@$(CC) -o $(TARGET) $(COBJ) $(LDFLAGS)

clean:
	@echo ========== cleanup ========== 
	rm -rf *.o $(TARGET)	
