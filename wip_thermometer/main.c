#include "one_wire.h"
#include "pca.h"

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define IMPL_LEDDSP_SERIAL 0
#define IMPL_STDOUT_SERIAL 1

#define IMPLEMENTATION IMPL_STDOUT_SERIAL


typedef enum _ge_chars {
	LED_SEG_0 = 0,
	LED_SEG_1,
	LED_SEG_2,
	LED_SEG_3, 
	LED_SEG_4,
	LED_SEG_5,
	LED_SEG_6,
	LED_SEG_7,
	LED_SEG_8,
	LED_SEG_9,
	LED_SEG_A,
	LED_SEG_C,
	LED_SEG_LSP,
	LED_SEG_F,
	LED_SEG_H,
	LED_SEG_L,
	LED_SEG_P,
	LED_SEG_U,
	LED_SEG_MINUS,
	LED_SEG_DEG,
	LED_SEG_o,
	LED_SEG_t,
	LED_SEG_n,
	LED_SEG_SPACE,

	LED_SEG_LAST
} ge_chars;


static const uint8_t g_chars[LED_SEG_LAST] PROGMEM = {
	0xfc, // 0
	0x14, // 1
	0xce, // 2
	0x9e, // 3
	0x36, // 4
	0xba, // 5
	0xfa, // 6
	0x1c, // 7
	0xfe, // 8
	0xbe, // 9
	0x7e, // A
	0xe8, // C
	0x9c, // ]
	0x6a, // F
	0x76, // H
	0xe0, // L
	0x6e, // P
	0xf4, // U
	0x02, // -
	0x2e, // *
	0xd2, // o
	0xe2, // t
	0x52, // n
	0x00
};


static volatile uint8_t g_digits[4] = {0x00};
static volatile uint8_t g_mux = 0x00;


ISR(TIMER0_COMPA_vect) {
	g_mux = (g_mux >= 3) ? 0 : (g_mux + 1);
	PORTD = 0x00;
	PORTB = (0x01 << g_mux);
	PORTD = g_digits[g_mux];
	
	if (PINC & _BV(PORTC1)) {
		PORTB |= _BV(PORTB5);
	}
	else {
		PORTB &= ~_BV(PORTB5);
	}
}


int main(void)
{
	// initialize serial interface
	/* serial_init(E_BAUD_9600);	 */
	/* serial_install_interrupts(SERIAL_RX_INTERRUPT); */
	/* serial_flush(); */
	/* serial_install_stdio(); */

	uint8_t scratchpad[9] = {0x00};
	uint8_t tmp_str[6] = {0x00};
	float temp = 0x00;
	ow_enable();

	DDRB = 0xff;
	DDRD = 0xff;

	// input for 1 Hz square wave from rtc
	DDRC &= ~_BV(PORTC1);

	PORTB = 0xff;
	PORTD = 0xff;

	_timer_init_ctc(E_TIMER0);
	uint32_t pocr = _timer_freq_prescale(E_TIMER0, 300, 255);
	_timer_setup_ctc(E_TIMER0, pocr);
	_timer_en_compa_int(E_TIMER0);

	g_digits[0] = pgm_read_byte(&g_chars[LED_SEG_0]);
	g_digits[1] = pgm_read_byte(&g_chars[LED_SEG_0]);
	g_digits[2] = pgm_read_byte(&g_chars[LED_SEG_0]);
	g_digits[3] = pgm_read_byte(&g_chars[LED_SEG_C]);

	sei();
	uint8_t cntr = 0;

	while (1) {
		ow_initialize();
		ow_write_byte(0xcc);
		ow_write_byte(0x44);

		ow_strong_pullup(1);
		_delay_ms(1000);

		ow_strong_pullup(0);
		ow_initialize();
		ow_write_byte(0xcc);
		ow_write_byte(0xbe);

		for (uint8_t x = 0; x<9; x++) scratchpad[x] = ow_read_byte();
		ow_initialize();

		temp = ((float)(scratchpad[0] | (scratchpad[1] << 8))/16);
		snprintf((char *)tmp_str, 5, "%2.1f", (double)temp);
		/* printf("Temp: %s\n", tmp_str); */

		g_digits[0] = pgm_read_byte(&g_chars[ (tmp_str[0] - '0') ]);
		g_digits[1] = pgm_read_byte(&g_chars[ (tmp_str[1] - '0') ]);
		g_digits[2] = pgm_read_byte(&g_chars[ (tmp_str[3] - '0') ]);
	}

	return 0;
}
