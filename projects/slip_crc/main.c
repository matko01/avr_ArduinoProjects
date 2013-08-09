#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <util/crc16.h>
#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "serial.h"
#include "slip.h"

char buffer[128] = {0};

uint16_t checkcrc(char *data) {
	uint16_t crc = 0x00;

	for (int16_t i = 0; i<strlen(buffer); i++) {
		crc = _crc16_update(crc, buffer[i]);
	}

	return crc;
}

int main(void)
{
	serial_init(E_BAUD_9600);	
	strcpy(buffer, "123456789");

	while(1) {
		char str[16] = {0x00};
	   	snprintf(str, sizeof(str), "CRC: %d", checkcrc(buffer));
		serial_send(str, strlen(str));
		_delay_ms(1000);
	}

	return 0;
}
