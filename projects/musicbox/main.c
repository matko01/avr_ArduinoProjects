#include "beeper.h"
#include "notes.h"
#include "serial.h"
#include "slip.h"

#include <avr/io.h>
#include <util/delay.h>

struct note {
	uint16_t note;
	uint16_t duration;
};

struct packet {
	uint16_t crc16;
	unsigned char num;
	struct note notes[4];
};

static void play(e_timer a_pin, struct note *a_note) {
	if (a_note->note || a_note->duration) {
		beeper_beep(a_pin, a_note->note, a_note->duration);
		beeper_block(a_pin);
	}
	else {
		/* beeper_off(a_pin); */
	}
}

/* #define PIN E_TIMER0 // PORTD6 */
#define PIN 0 // PORTD6

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
		if (size < 5) {
			continue;
		}

		// verify if the information is genuine
		if (!slip_verify_crc16((unsigned char *)&pack, size, 0)) {
			continue;
		}

		for (unsigned char i = 0; i < pack.num; i++) {
			play(PIN, &pack.notes[i]);
			serial_poll_send(&pack.notes[i].note, 2);
			serial_poll_send(&pack.notes[i].duration, 2);
		}
	}

	return 0;
}
