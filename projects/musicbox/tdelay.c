#include "tdelay.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <stdint.h>

/* ================================================================================ */

static volatile uint32_t g_tc[E_TIMER_LAST] = {0x00};

/* ================================================================================ */

#ifdef TIMER_DELAY_ISR_2
/**
 * @brief timer 2
 *
 * @param TIMER2_COMPA_vect
 */
ISR(TIMER2_COMPA_vect) {
	if (!g_tc[E_TIMER2]--) {
		// MASK the interrupt + disable the clock;		
		TCCR2B = 0x00;
		TIMSK2 &= ~_BV(OCIE2A);
	}
}
#endif

#ifdef TIMER_DELAY_ISR_1
/**
 * @brief timer 1
 *
 * @param TIMER1_COMPA_vect
 */
ISR(TIMER1_COMPA_vect) {
	if (!g_tc[E_TIMER1]--) {
		// MASK the interrupt + disable the clock;		
		TCCR1B &= 0xf8;
		TIMSK1 &= ~_BV(OCIE1B);
	}
}
#endif

#ifdef TIMER_DELAY_ISR_0
/**
 * @brief timer 0
 *
 * @param TIMER0_COMPA_vect
 */
ISR(TIMER0_COMPA_vect) {
	if (!g_tc[E_TIMER0]--) {
		// MASK the interrupt + disable the clock;		
		TCCR0B = 0x00;
		TIMSK0 &= ~_BV(OCIE0A);
	}
}
#endif

/* ================================================================================ */

void tdelay_init(e_timer a_timer) {

	// enable interrupts
	sei();

	switch (a_timer) {
		case E_TIMER0:
			power_timer0_enable();
			TCCR0A = 0x02;
			OCR0A = 0x00;

			// clock disabled
			TCCR0B = 0x00;
			TCNT0 = 0x00;
			break;

		case E_TIMER1:
			power_timer1_enable();
			TCCR1A = 0x00;

			// clock disabled, CTC mode
			TCCR1B = 0x08;
			OCR1AH = 0x00;
			OCR1AL = 0x00;
			TCNT1L = 0x00;
			TCNT1H = 0x00;
			break;

		case E_TIMER2:
			power_timer2_enable();
			TCCR2A = 0x02;
			OCR2A = 0x00;
			TCCR2B = 0x00;
			TCNT2 = 0x00;
			break;

		default:
			break;
	} // switch
	g_tc[a_timer] = 0x00;
}

void tdelay_ms(e_timer a_timer, uint32_t a_delay)
{
	// which pin
	switch(a_timer) {

		case E_TIMER0:
			// interrupt every 1ms
			TCCR0B &= 0xf8;
			TCCR0B |= 0x03;			
			OCR0A = 250;
			g_tc[a_timer] = a_delay;
			TIMSK0 |= _BV(OCIE0A);
			break;

		case E_TIMER1:
			TCCR1B &= 0xf8;
			TCCR1B |= 0x03;
			// setup delay counter
			g_tc[a_timer] = a_delay;
			OCR1AH = 0x00;
			OCR1AL = 250;
			TIMSK1 |= _BV(OCIE1A);
			break;

		case E_TIMER2:
			// interrupt every 1ms
			TCCR2B &= 0xf8;
			TCCR2B |= 0x03;			
			OCR2A = 250;
			g_tc[a_timer] = a_delay;
			TIMSK2 |= _BV(OCIE2A);
			break;

		default:
			return;
			break;
	} // switch

	// sleep until the interrupt zero's the timer
	while(g_tc[a_timer]);
}

/* ================================================================================ */

