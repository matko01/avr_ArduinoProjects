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
	unsigned char num;
	struct note notes[32];
};

static void play(e_timer_pins a_pin, struct note *a_note) {
	if (a_note->note || a_note->duration) {
		beeper_beep(a_pin, a_note->note, a_note->duration);
		beeper_block(a_pin);
	}
	else {
		beeper_off(a_pin);
	}
}

int main(void)
{
	serial_init(E_BAUD_9600);	
	beeper_init(PD6_OC0A);

	unsigned char size = 0;
	struct packet pack;

	while (1) {
		if (0 >= (size = slip_recv((void *)&pack, sizeof(pack))))
			continue;

		// ignore incomplete data chunks
		if (size < 5) {
			continue;
		}

		for (unsigned char i = 0; i < pack.num; i++) {
			play(PD6_OC0A, &pack.notes[i]);
			/* serial_send(&size, 1); */
			/* serial_send(&pack.notes[i].note, 2); */
			/* serial_send(&pack.notes[i].duration, 2); */

		}
	}

	return 0;
}
