#include <avr/io.h>
#include <math.h>
#include <util/delay.h>

#include "display.h"

unsigned char screen[5] = {0};
unsigned char buffer[5] = {0};

t_display_dev dev = {
	.rows = {
		{ &PORTD, 0 },
		{ &PORTD, 1 },
		{ &PORTD, 2 },
		{ &PORTD, 3 },
		{ &PORTD, 4 }
	},

	.cols = {
		{ &PORTB, 0 },
		{ &PORTB, 1 },
		{ &PORTB, 2 },
		{ &PORTB, 3 },
		{ &PORTB, 4 }
	}

};

unsigned char x = 0;
unsigned char y = 0;
unsigned char cnt = 0;

static void loop() {
	display_clear(buffer);
	display_putpixel(buffer, MAKE_COORD(x,y,1));

	if (cnt > 10) {
		x++;
		x = x % DISPLAY_WIDTH;

		if (!x) y++;
		y = y % DISPLAY_HEIGHT;		
	}
	else {
		y++;
		y = y % DISPLAY_HEIGHT;

		if (!y) x++;
		x = x % DISPLAY_WIDTH;
		
	}

	cnt++;
	cnt = cnt % 20;

	display_blit(buffer);
	_delay_ms(50);
}

int main(void)
{
	display_set(buffer);
	display_install(&dev);

	while (1) {
		loop();
	}

	return 0;
}
