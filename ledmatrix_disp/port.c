#include <avr/io.h>

#include "port.h"

void pin_conf_output(volatile struct t_pin *a_pin) {
	if (a_pin->port == &PORTB) {
		DDRB |= _BV(a_pin->pin);
	}	
	else if (a_pin->port == &PORTC) {
		DDRC |= _BV(a_pin->pin);
	} 
	else if (a_pin->port == &PORTD) {
		DDRD |= _BV(a_pin->pin);
	} 
}
