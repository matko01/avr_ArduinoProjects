#include "tdelay.h"

#include <avr/power.h>
#include <avr/interrupt.h>


void tdelay_init(e_timer a_timer) {

	// enable interrupts
	sei();
	switch (a_timer) {
		case E_TIMER0:
			power_timer0_enable();
			// clock disabled
			TCCR0A = 0x02;
			TCCR0B = 0x00;
			break;

		case E_TIMER1:
			power_timer1_enable();
			// clock disabled, CTC mode
			TCCR1A = 0x00;
			TCCR1B = 0x08;
			break;

		case E_TIMER2:
			power_timer2_enable();
			TCCR2A = 0x02;
			TCCR2B = 0x00;
			break;

		default:
			break;
	} // switch

	_tdc_set_duration(a_timer, 0x00);
	_tdc_set_cmp_pin(a_timer, 0x00);
}

void tdelay_ms(e_timer a_timer, uint32_t a_delay) {
	_tdc_set_cmp_pin(a_timer, 0x00);
	_tdc_setup_ms(a_timer, a_delay);
	_tdc_enable_interrupt(a_timer);
	_tdc_block(a_timer);
}

