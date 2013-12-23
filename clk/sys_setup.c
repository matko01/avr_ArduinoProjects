#include "sys_setup.h"
#include "sys_config.h"

#include <avr/power.h>


void bus_ow_setup(struct soft_ow *a_sow_ctx) {

	// soft one wire bus setup
	a_sow_ctx->conf = 
		SOFT_OW_CONF_MAKE(OW_POWER_PARASITE, OW_SINGLEDROP);

	a_sow_ctx->bus.port = &OW_PORT;
	a_sow_ctx->bus.pin = OW_PIN;

	// initialize one-wire
	sow_init(a_sow_ctx);
}


void bus_twi_setup(struct twi_ctx **a_ctx) {
	// I2C CTX
	*a_ctx = (struct twi_ctx *)twi_init(E_TWI_SCL_100K);
}


void led_setup(gpio_pin *led) {
	led->port = &RTC_LED_PORT;
	led->pin = RTC_LED_PIN;

	GPIO_CONFIGURE_AS_OUTPUT(led);
	GPIO_SET_HIGH(led);
}


void timers_setup() {
	power_timer0_enable();
	power_timer2_enable();

	// fast PWM mode on OC2A (PB3) - OC2B (PD3)
	// in non inverting mode
	// prescaler = 256
	TCCR2A = 0xa3;
	TCCR2B = 0x01;

	// timer 0 in normal mode (61 Hz)
	// will be used as a time source
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	TIMSK0 = 0x01;

	// enable external int0 (pd2) on falling edge
	// RTC 1 HZ output
	EICRA = 0x02;
	EIMSK = 0x01;
	PORTD &= ~_BV(PORTD2);
	DDRD &= ~_BV(PORTD2);

	SET_CONTRAST(0x00);
	SET_BRIGHTNESS(0x00);

	// enable output on PORTB3	
	DDRB |= _BV(PORTB3);
	DDRD |= _BV(PORTD3);

	DDRC &= 0xf0; // configure first PORTC ports as inputs
	PORTC |= 0x0f; // enable pull-ups 
}


