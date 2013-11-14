#include "one_wire.h"
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <util/atomic.h>


#define OW_OUTPUT \
	OW_DDR |= _BV(OW_PIN)


#define OW_INPUT \
	OW_DDR &= ~_BV(OW_PIN)


#define OW_HIGH \
	OW_PORTO |= _BV(OW_PIN)


#define OW_LOW \
	OW_PORTO &= ~_BV(OW_PIN)


void ow_enable() {
	// disable pull-ups
	MCUCR |= _BV(PUD);
	OW_INPUT;
	_delay_us(500);
}


void ow_strong_pullup(uint8_t a_enable) {
	if (a_enable) {
		OW_OUTPUT;
		OW_HIGH;
		return;
	}

	OW_INPUT;
}


uint8_t ow_initialize() {

	uint8_t presence = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		OW_OUTPUT;
		OW_LOW;
		_delay_us(500);

		OW_INPUT;
		_delay_us(50); // 15 - 60

		if (bit_is_clear(OW_PORTI, OW_PIN)) {
			// after 60 - 240
			_delay_us(300); 
			if (bit_is_set(OW_PORTI, OW_PIN)) {
				presence = 1;
			}
		}
	}
	
	return presence;
}


uint8_t ow_write_bit(uint8_t a_bit) {
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		OW_OUTPUT;
		OW_LOW;
		_delay_us(5); // max 15
		if (a_bit & 0x01)
			OW_INPUT;

		_delay_us(80);
		OW_INPUT;
	}
	return 1;
}


uint8_t ow_read_bit() {
	uint8_t bit = 0;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		OW_OUTPUT;
		OW_LOW;
		_delay_us(1); // >= 1
		OW_INPUT;

		_delay_us(12); // sample within 15u slot
		if (bit_is_set(OW_PORTI, OW_PIN))
			bit = 1;

		// time slot duration
		_delay_us(60);

		// recovery time
		_delay_us(1); 
	}
	return bit;
}


uint8_t ow_write_byte(uint8_t a_byte) {
	uint8_t n = 0;

	for (; n<8; n++) {
		ow_write_bit( ((a_byte >> n) & 0x01) );
	}
	
	return 1;
}


uint8_t ow_read_byte() {
	uint8_t n = 0;
	uint8_t data = 0x00;

	for (; n<8; n++) {
		if (ow_read_bit()) data |= (0x01 << n);
	}
	
	return data;
}


uint8_t ow_write_data(uint8_t *a_data, uint8_t a_len) {
	uint8_t n = a_len;

	while (a_len--) {
		ow_write_byte(*a_data);
		a_data++;
	}

	return n;
}


uint8_t ow_read_data(uint8_t *a_data, uint8_t a_maxlen) {
	uint8_t n = a_maxlen;

	while (a_maxlen--) {
		*a_data = ow_read_byte();
		a_data++;
	}

	return n;
}

