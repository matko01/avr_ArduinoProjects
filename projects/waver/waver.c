#include <avr/io.h>
#include <util/delay.h>

unsigned char data[] = {
	0x01 << 0,
	0x01 << 1,
	0x01 << 2,
	0x01 << 3,
	0x01 << 4, 
	0x01 << 5, 
	0x01 << 4,
	0x01 << 3,
	0x01 << 2,
	0x01 << 1,
};

unsigned char lines = sizeof(data);

struct port_ctx {
	unsigned char port;
	unsigned char pin;
};

struct port_ctx cfg[] = {
	{ 'D', DDD1 },
	{ 'D', DDD3 },
	{ 'D', DDD5 },
	{ 'D', DDD7 },
	{ 'B', DDB1 },
	{ 'B', DDB3 }
};


void pin_setup(struct port_ctx *a_port) {
	switch (a_port->port) {
		case 'B':
			DDRB |= _BV(a_port->pin);
			break;

		case 'C':
			DDRC |= _BV(a_port->pin);
			break;

		case 'D':
			DDRD |= _BV(a_port->pin);
			break;

		default:
			break;
	} // switch
}


void pin_set(struct port_ctx *a_port, unsigned char a_value) {
	switch (a_port->port) {
		case 'B':
			if (a_value) 
				PORTB |= _BV(a_port->pin);
			else 
				PORTB &= ~_BV(a_port->pin);
			break;

		case 'C':
			if (a_value)
				PORTC |= _BV(a_port->pin);
			else
				PORTC &= ~_BV(a_port->pin);
			break;

		case 'D':
			if (a_value) 
				PORTD |= _BV(a_port->pin);
			else
				PORTD &= ~_BV(a_port->pin);
			break;

		default:
			break;
	} // switch

}

void port_zero(unsigned char a_port) {
	switch (a_port) {
		case 'B':
			PORTB = 0x00;
			break;

		case 'C':
			PORTC = 0x00;
			break;

		case 'D':
			PORTD = 0x00;
			break;

		default:
			break;
	} // switch
}

void setup() {
	unsigned char cnt = 0;
	for (; cnt < (sizeof(cfg)/sizeof(struct port_ctx)); cnt++) {
		pin_setup(&cfg[cnt]);
	} // for
}

void display_line(unsigned char a_line) {
	unsigned char cnt = 0;
	for (; cnt < 6; cnt++) {
		pin_set(&cfg[cnt], a_line & 0x01);
		a_line >>= 1;
	}
}

void loop()
{
	unsigned char cnt = 0;
	for (; cnt < lines; cnt++) {
		display_line(data[cnt]);
		_delay_ms(60);
		/* _delay_ms('2'); */
	}
}

int main(void)
{
	setup();
	while (1) {
		loop();
	}
	return 0;
}
