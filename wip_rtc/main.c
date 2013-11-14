#include "pca.h"


#include <stdio.h>
#include <string.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/interrupt.h>


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
	return a_data;
}


uint8_t twi_mr_read_data(uint8_t a_dev_addr, uint8_t *a_data, uint8_t a_len) {

	// do nothing if the request is for zero bytes or the 
	// destination buffer is not available
	if (NULL == a_data || !a_len)
		return;

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
	return a_data;
}


ISR(INT0_vect) {
	if (PIND & 0x04) {
		PORTB |= 0x20;
	}
	else {
		PORTB &= 0xdf;
	}
}


int main(void)
{
	uint8_t ds1307_addr = 0xd0;

	// initialize serial interface
	serial_init(E_BAUD_9600);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();

	/* DDRB |= 0x20; */
	/* PORTB &= 0xdf; */

	// enable INT0 as input
	/* DDRD &= 0xfb; */
	/* EIMSK = 0x01; */

	// any logical changes triggers an int
	/* EICRA = 0x01; */

	// power up the twi interface
	power_twi_enable();

	// prescaler = 4
	TWSR = 0x01;
	TWBR = 0x12;

	// enable two wire interface
	TWCR |= _BV(TWEN);

	// wpisz data ptr jakos 0x00
	// odczytaj

	uint8_t data[8] = {0x00};

	// enable 1 Hz output
	data[0] = 0x90;
	twi_mt_write_data(ds1307_addr, 0x07, data, 0x01);

	// clear data buffer
	memset(data, 0x00, sizeof(data));

	while (1) {
		// SLA + W
		twi_mt_write_data(ds1307_addr, 0x00, NULL, 0x00);
		
		// send info
		printf("ptr = 0x00 has been sent successfully\n");

		// get data
		twi_mr_read_data(ds1307_addr, data, 0x08);

		// dump data to serial console
		for (uint8_t i = 0; i<8; i++) {
			printf("Data received: %02d -> %02x\n", i, data[i]);
		}

		_delay_ms(900);
	}

	return 0;
}
