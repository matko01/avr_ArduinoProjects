#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <util/crc16.h>
#include <util/delay.h>
#include <string.h>

#include "main.h"
#include "serial.h"
#include "slip.h"

unsigned char buffer[128] = {0};

uint16_t checkcrc(unsigned char *data, unsigned char len) {
	uint16_t crc = 0x00;

	for (int16_t i = 0; i<len; i++) {
		crc = _crc16_update(crc, buffer[i]);
	}

	return crc;
}


#define	TEST_SIMPLE 0
#define	TEST_RECEIVE 1
#define TEST_SEND 2
#define TEST_VERIFY 3

#define TEST_TYPE TEST_RECEIVE

int main(void)
{
	serial_init(E_BAUD_4800);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_flush();

	while(1) {
		char str[32] = {0x00};
		memset(str, 0x00, sizeof(str));

#if TEST_TYPE == TEST_SIMPLE
#warning Building Test Simple

		strcpy(buffer, "123456789");
	   	snprintf(str, sizeof(str), "CRC: %04x\n", 
				checkcrc((unsigned char *)buffer, strlen(buffer)));
		serial_poll_send(str, strlen(str));

#elif TEST_TYPE == TEST_RECEIVE
#warning Building Test Receive

		unsigned char size = 0x00;
		uint16_t crc = 0x0000;

		if (0 >= (size = slip_recv((unsigned char *)buffer, 128)))
			continue;

		memcpy(&crc, &buffer[size - 2], 2);
		memset(&buffer[size - 2], 0x00, 2);

	   	snprintf(str, sizeof(str), "RECV [%04x], CRC: %04x\r\n", 
				crc,
				checkcrc((unsigned char *)buffer, size));
		serial_poll_send(str, strlen(str));

#elif TEST_TYPE == TEST_VERIFY
#warning Building Test Verify

		unsigned char size = 0x00;

		if (0 >= (size = slip_recv((unsigned char *)buffer, 128)))
			continue;

	   	snprintf(str, sizeof(str), "Verification: [%s]\r\n", 
				slip_verify_crc16(buffer, size, size - 2) ?
				"positive" : "negative");

		serial_poll_send(str, strlen(str));

#elif TEST_TYPE == TEST_SEND
#warning Building Test Send

		strcpy(buffer, "123456789");
		slip_append_crc16((unsigned char *)buffer, strlen(buffer));
		serial_poll_send(buffer, 11);
		serial_poll_send("\n",1);

#else
#error Uknown test type
#endif
		_delay_ms(200);
	}

	return 0;
}
