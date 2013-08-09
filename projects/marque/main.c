#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#include "display.h"
#include "marquee.h"
#include "serial.h"
#include "cli.h"

/**
 * @brief display pin configuration
 */
t_display_dev dev = {
	.rows = {
		{ &PORTD, 3 },
		{ &PORTD, 4 },
		{ &PORTD, 5 },
		{ &PORTD, 6 },
		{ &PORTD, 7 }
	},

	.cols = {
		{ &PORTB, 0 },
		{ &PORTB, 1 },
		{ &PORTB, 2 },
		{ &PORTB, 3 },
		{ &PORTB, 4 }
	}
};


int main(void)
{
	display_set(0x00);
	display_install(&dev);

	marque_init();	
	marque_set_delay(0x3500);
	serial_init(E_BAUD_4800);
	marque_print("HELLO");
	cli_init();

	while (1) {
		marque_scroll();
		cli_read();
	}

	return 0;
}
