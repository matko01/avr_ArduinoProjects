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

uint16_t checkcrc(unsigned char *data, unsigned char len) {
	uint16_t crc = 0x00;

	for (int16_t i = 0; i<len; i++) {
		crc = _crc16_update(crc, buffer[i]);
	}

	return crc;
}


#define	TEST_SIMPLE 0
#define	TEST_RECEIVE 1


#define TEST_TYPE TEST_RECEIVE

int main(void)
{
	serial_init(E_BAUD_9600);	

	while(1) {
		char str[32] = {0x00};
		memset(str, 0x00, sizeof(str));

#if TEST_TYPE == TEST_SIMPLE
#warning Building Test Simple

		strcpy(buffer, "123456789");
	   	snprintf(str, sizeof(str), "CRC: %04x", 
				checkcrc((unsigned char *)buffer, strlen(buffer)));
		serial_send(str, strlen(str));

#elif TEST_TYPE == TEST_RECEIVE
#warning Building Test Receive

		unsigned char size = 0x00;
		uint16_t crc = 0x0000;

		memcpy(&crc, buffer, 2);
		memset(buffer, 0x00, 2);

		if (0 >= (size = slip_recv((unsigned char *)buffer, sizeof(buffer))))
			continue;

	   	snprintf(str, sizeof(str), "RECV [%04x], CRC: %04x", 
				crc,
				checkcrc((unsigned char *)buffer, size));
		serial_send(str, strlen(str));

#else
#error Uknown test type
#endif
		_delay_ms(1000);
	}

	return 0;
}
