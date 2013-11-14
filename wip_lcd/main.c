#include "pca.h"
#include "hd44780.h"

#include <util/delay.h>
#include <avr/power.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>

#define twi_start \
	TWCR = (_BV(TWSTA) | _BV(TWINT) | _BV(TWEN))

#define twi_twint_wait \
	while (!(TWCR & _BV(TWINT)))

#define twi_send \
	TWCR = (_BV(TWINT) | _BV(TWEN))

#define twi_stop \
	TWCR = (_BV(TWSTO) | _BV(TWINT) | _BV(TWEN))

#define twi_status_get \
	(TWSR & 0xf8)

#define STATUS_START 0x08
#define STATUS_REPEATED_START 0x10
#define STATUS_MT_SLA_ACK 0x18
#define STATUS_MT_DATA_ACK 0x28
#define STATUS_MR_SLA_ACK 0x40

void twi_start_gen() {
	twi_start;
	twi_twint_wait;
}


void _twi_write_byte(uint8_t a_byte) {
	TWDR = a_byte;
	twi_send;
	twi_twint_wait;

	switch (twi_status_get) {
		// TODO implement error handling
		default:
		case STATUS_MT_SLA_ACK:
		case STATUS_MT_DATA_ACK:
		case STATUS_MR_SLA_ACK:
			// everything is fine, no error
			break;
	}
}


uint8_t twi_mt_write_data(uint8_t a_dev_addr, uint8_t a_addr, uint8_t *a_data, uint8_t a_len) {

	twi_start_gen();

	switch (twi_status_get) {
		// TODO implement error handling
		default:
		case STATUS_START:
			break;
	}

	// SLA + W
	_twi_write_byte((a_dev_addr & 0xfe));

	// send address
	_twi_write_byte(a_addr);

	if (a_data) {
		// send data burst
		while (a_len) {
			_twi_write_byte(*a_data);
			a_data++;
			a_len--;
		}
	}

	twi_stop;
	return a_len;
}


uint8_t twi_mr_read_data(uint8_t a_dev_addr, uint8_t *a_data, uint8_t a_len) {

	// do nothing if the request is for zero bytes or the 
	// destination buffer is not available
	if (NULL == a_data || !a_len)
		return 0;

	twi_start_gen();

	switch (twi_status_get) {
		// TODO implement error handling
		default:
		case STATUS_START:
			break;
	}

	// SLA + W
	_twi_write_byte((a_dev_addr | 0x01));

	// get data
	while (a_len) {
		if (0x01 == a_len) {
			// send data byte + NOT ACK
			TWCR &= ~_BV(TWEA);
		}
		else {	
			TWCR |= _BV(TWEA);	
		}

		twi_twint_wait;
		*a_data = TWDR;

		a_data++;
		a_len--;
	}

	twi_stop;
	return a_len;
}


int main(void)
{
	uint8_t ds1307_addr = 0xd0;

	uint8_t rtc_data[8] = {0x00};
	uint8_t scratchpad[9] = {0x00};
	uint8_t tmp_str[32] = {0x00};
	uint8_t tmp_str2[16] = {0x00};
	float temp = 0x00;

	struct soft_ow ds18b20;

	ds18b20.pin = PIND7;
	ds18b20.ddr = &DDRD;
	ds18b20.inp = &PIND;
	ds18b20.outp = &PORTD;

	lcd_init();
	lcd_clrscr();

	sow_init(&ds18b20);

	// turn on blinking cursor
	lcd_command(_BV(LCD_DISPLAYMODE) | _BV(LCD_DISPLAYMODE_ON) | _BV(LCD_DISPLAYMODE_BLINK));

	// power up the twi interface
	power_twi_enable();

	// prescaler = 4
	TWSR = 0x01;
	TWBR = 0x12;

	// enable two wire interface
	TWCR |= _BV(TWEN);

	// enable clock + set clock
	rtc_data[0] = 0x30;
	rtc_data[1] = 0x57;
	rtc_data[2] = 0x22;
	twi_mt_write_data(ds1307_addr, 0x00, rtc_data, 0x03);

	// enable 1 Hz output
	rtc_data[0] = 0x90;
	twi_mt_write_data(ds1307_addr, 0x07, rtc_data, 0x01);

	while(1) {

		sow_reset(&ds18b20);
		sow_write_byte(&ds18b20, 0xcc);
		sow_write_byte(&ds18b20, 0x44);

		sow_strong_pullup(&ds18b20, 1);
		_delay_ms(800);
		sow_strong_pullup(&ds18b20, 0);

		sow_reset(&ds18b20);
		sow_write_byte(&ds18b20, 0xcc);
		sow_write_byte(&ds18b20, 0xbe);

		for (uint8_t x = 0; x<9; x++) 
			scratchpad[x] = sow_read_byte(&ds18b20);

		sow_reset(&ds18b20);

		// SLA + W
		twi_mt_write_data(ds1307_addr, 0x00, NULL, 0x00);
		
		// get data
		twi_mr_read_data(ds1307_addr, rtc_data, 0x08);

		temp = ((float)(scratchpad[0] | (scratchpad[1] << 8))/16);
		snprintf((char *)tmp_str, sizeof(tmp_str), "Temp: %2.3f [C]", (double)temp);
		snprintf((char *)tmp_str2, sizeof(tmp_str2), "Time: %2x:%02x:%02x", rtc_data[2], rtc_data[1], rtc_data[0]);

		lcd_goto(0x00);
		lcd_puts((char *)tmp_str);

		lcd_goto(0x40);
		lcd_puts((char *)tmp_str2);
	}

	return 0;
}
