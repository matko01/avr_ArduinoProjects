#include "serial.h"
#include "slip.h"

#include <avr/io.h>
#include <util/delay.h>


struct packet {
	uint16_t crc16;
	uint16_t num;
	uint8_t samplerate;
	uint8_t samples[1024];
};


#define PIN E_TIMER1 // PORTD6

int main(void)
{
	serial_init(E_BAUD_38400);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_flush();

	beeper_init(PIN);

	unsigned char size = 0;
	struct packet pack;

	while (1) {

		if (0 >= (size = slip_recv((void *)&pack, sizeof(pack)))) {
			continue;
		}

		// ignore incomplete data chunks
		if (size < 6) {
			continue;
		}

		// verify if the information is genuine
		if (!slip_verify_crc16((unsigned char *)&pack, size, 0)) {
			continue;
		}



		for (unsigned char i = 0; i < pack.num; i++) {
			// play the samples
		}
	}

	return 0;
}
