#include "beeper.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <stdint.h>

/* ================================================================================ */

static volatile uint32_t g_tc[TIMER_PIN_LAST] = {0x00};

/* ================================================================================ */

#ifdef TIMER_BEEPER_ISR_2

/**
 * @brief timer 2
 *
 * @param TIMER2_COMPA_vect
 */
ISR(TIMER2_COMPA_vect) {
	if (!g_tc[PB3_OC2A]) {
		// MASK the interrupt + disable the clock;		
		TIMSK2 &= ~_BV(OCIE2A);
		TCCR2B = 0x00;
		PORTB &= ~_BV(PORTB3);
	}
	else {
		if (g_tc[PB3_OC2A]) g_tc[PB3_OC2A]--;
	}
}

/**
 * @brief timer 2
 *
 * @param TIMER2_COMPB_vect
 */
ISR(TIMER2_COMPB_vect) {
	if (!g_tc[PD3_OC2B]) {
		// MASK the interrupt + disable the clock;		
		TIMSK2 &= ~_BV(OCIE2B);
		TCCR2B = 0x00;
		PORTD &= ~_BV(PORTD3);
	}
	else {
		if (g_tc[PD3_OC2B]) g_tc[PD3_OC2B]--; 
	}
}
#endif


#ifdef TIMER_BEEPER_ISR_1
/**
 * @brief timer 1
 *
 * @param TIMER1_COMPA_vect
 */
ISR(TIMER1_COMPA_vect) {
	if (!g_tc[PB1_OC1A]) {
		// MASK the interrupt + disable the clock;		
		TIMSK1 &= ~_BV(OCIE1B);
		TCCR1B &= 0xf8;
		PORTB &= ~_BV(PORTB1);
	}
	else {
		if (g_tc[PB1_OC1A]) g_tc[PB1_OC1A]--;
	}
}

/**
 * @brief timer 1
 *
 * @param TIMER1_COMPB_vect
 */
ISR(TIMER1_COMPB_vect) {
	if (!g_tc[PB2_OC1B]) {
		// MASK the interrupt + disable the clock;		
		TIMSK1 &= ~_BV(OCIE1B);
		TCCR1B &= 0xf8;
		PORTB &= ~_BV(PORTB2);
	}
	else {
		if (g_tc[PB2_OC1B]) g_tc[PB2_OC1B]--;
	}
		
}
#endif


#ifdef TIMER_BEEPER_ISR_0
/**
 * @brief timer 0
 *
 * @param TIMER0_COMPA_vect
 */
ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
	if (!g_tc[PD6_OC0A]) {
		// MASK the interrupt + disable the clock;		
		TCCR0B = 0x00;
		TIMSK0 &= ~_BV(OCIE0A);
		PORTD &= ~_BV(PORTD6);
	}
	else {
		if (g_tc[PD6_OC0A]) g_tc[PD6_OC0A]--;
	}
}

/**
 * @brief timer 0
 *
 * @param TIMER0_COMPB_vect
 */
ISR(TIMER0_COMPB_vect, ISR_NOBLOCK) {
	if (!g_tc[PD5_OC0B]) {
		// MASK the interrupt + disable the clock;		
		TCCR0B = 0x00;
		TIMSK0 &= ~_BV(OCIE0B);
		PORTD &= ~_BV(PORTD5);
	}
	else {
		if (g_tc[PD5_OC0B]) g_tc[PD5_OC0B]--;
	}
}
#endif

/* ================================================================================ */

void beeper_init(e_timer_pins a_pin) {

	// enable interrupts
	sei();

	switch (a_pin) {
		case PD6_OC0A:
		case PD5_OC0B:

			power_timer0_enable();

			// clock disabled
			TCCR0B = 0x00;
			TCNT0 = 0x00;

			if (PD6_OC0A == a_pin) {
				TCCR0A = 0x42;
				OCR0A = 0x00;
				DDRD |= _BV(PORTD6);
			}
			else {
				TCCR0A = 0x12;
				OCR0B = 0x00;
				DDRD |= _BV(PORTD5);
			}


			break;

		case PB1_OC1A:
		case PB2_OC1B:
			power_timer1_enable();
			if (PB1_OC1A) {
				DDRB |= _BV(PORTB1);
				TCCR1A = 0x40;
			}
			else {
				DDRB |= _BV(PORTB2);
				TCCR1A = 0x10;
			}

			// clock disabled, CTC mode
			TCCR1B = 0x08;
			OCR1AH = 0x00;
			OCR1AL = 0x00;
			TCNT1L = 0x00;
			TCNT1H = 0x00;
			break;

		case PB3_OC2A:
		case PD3_OC2B:
			power_timer2_enable();
			if (PB3_OC2A) {
				DDRB |= _BV(PORTB3);
				TCCR2A = 0x42;
				OCR2A = 0x00;
			}
			else {
				DDRD |= _BV(PORTD3);
				TCCR2A = 0x12;
				OCR2B = 0x00;
			}

			TCCR2B = 0x00;
			TCNT2 = 0x00;
			break;

		default:
			break;
	} // switch
}

/**
 * @brief calculate prescaler and OCR value for 8 bit counters
 *
 * @param a_freq requested frequency
 * @param a_ocr ocr value returned by reference
 * @param a_prescaler prescaler value returned by reference
 */
static void timer8_prescaler(uint32_t a_freq, 
		uint8_t *a_ocr, 
		uint8_t *a_prescaler) {

	uint32_t ocr = 0x00;
	uint8_t prescalers[] = { 0, 3, 6, 8, 10 };
	uint8_t pnum = sizeof(prescalers);
	uint8_t i = 0;

	// small achievable frequency with this timer
	if (a_freq < 62) {
		*a_prescaler = 5;
		*a_ocr = 0;
		return;
	}

	do {
		ocr = F_CPU / ((a_freq << 1) * (0x01 << prescalers[i]));
	} while ((ocr > 255) && (++i < pnum));

	*a_ocr = (ocr & 0xff);	
	*a_prescaler = i;
}

/**
 * @brief calculate prescaler and ocr values for 16 bit counter
 *
 * @param a_freq requested frequency
 * @param a_ocrh ocr high
 * @param a_ocrl ocr low
 * @param a_prescaler prescaler value
 */
static void timer16_prescaler(uint32_t a_freq,
		uint8_t *a_ocrh,
		uint8_t *a_ocrl,
		uint8_t *a_prescaler) {

	uint32_t ocr = 0x00;
	uint8_t prescalers[] = { 0, 3, 6, 8, 10 };
	uint8_t pnum = sizeof(prescalers);
	uint8_t i = 0;

	do {
		ocr = F_CPU / (a_freq << (1 + prescalers[i]));
	} while ((ocr > 65535) && (++i < pnum));

	*a_ocrh = (ocr & 0xff00) >> 8;
	*a_ocrl = (ocr & 0xff);
	*a_prescaler = i;
}

void beeper_beep(e_timer_pins a_pin, 
	uint32_t freq, 
	uint32_t duration)
{
	uint8_t ocr = 0x00;
	uint8_t ocrh = 0x00;
	uint8_t presc = 0x00;

	beeper_off(a_pin);

	// which pin
	switch(a_pin) {

		case PD6_OC0A:
		case PD5_OC0B:
			TCCR0B &= 0xf8;

			if (freq) {
				TCCR0A |= a_pin == PD6_OC0A ? 0x40 : 0x10;
				timer8_prescaler(freq, &ocr, &presc);

				TCCR0B |= (presc & 0x07);

				freq = freq < 62 ? 62 : freq;
				// setup delay counter
				g_tc[a_pin] = (uint32_t)((freq*duration) / 500);

			}
			else {
				TCCR0A &= 0x0f;
				TCCR0B |= 0x03;
				ocr = 250;
				g_tc[a_pin] = duration;
			}

			if (PD6_OC0A == a_pin) {
				OCR0A = ocr;
				TIMSK0 |= _BV(OCIE0A);
			}
			else {
				OCR0B = ocr;
				TIMSK0 |= _BV(OCIE0B);
			}
			break;

		case PB1_OC1A:
		case PB2_OC1B:
			TCCR1B &= 0xf8;
			if (freq) {
				timer16_prescaler(freq, &ocrh, &ocr, &presc);
				TCCR0A |= (PB1_OC1A == a_pin ? 0x40 : 0x10);
				TCCR1B |= (presc & 0x07);

				// setup delay counter
				g_tc[a_pin] = (freq*duration) / 500;
			}
			else {
				TCCR0A &= 0x0f;
				TCCR1B |= 0x03;
				ocrh = 0;
				ocr = 250;
				g_tc[a_pin] = duration;
			}

			if (PB1_OC1A == a_pin) {
				OCR1AH = ocrh;
				OCR1AL = ocr;
				TIMSK1 |= _BV(OCIE1A);
			}
			else {
				OCR1BH = ocrh;
				OCR1BL = ocr;
				TIMSK1 |= _BV(OCIE1B);
			}
			break;

		case PB3_OC2A:
		case PD3_OC2B:
			TCCR2B &= 0xf8;
			if (freq) {
				timer8_prescaler(freq, &ocr, &presc);
				TCCR2A |= (a_pin == PB3_OC2A ? 0x40 : 0x10);
				TCCR2B |= (presc & 0x07);

				// setup delay counter
				g_tc[a_pin] = (freq*duration) / 500;
			}
			else {
				TCCR2A &= 0x0f;
				TCCR2B |= 0x03;
				ocr = 250;
				g_tc[a_pin] = duration;
			}

			if (PB3_OC2A == a_pin) {
				OCR2A = ocr;
				TIMSK2 |= _BV(OCIE2A);
			}
			else {
				OCR2B = ocr;
				TIMSK2 |= _BV(OCIE2B);
			}
			break;

		default:
			return;
			break;
	} // switch
}

void beeper_block(e_timer_pins a_pin) {
	unsigned char wait = 0;

	// deadlock protection
	switch (a_pin) {
		case PD6_OC0A:
			if (bit_is_clear(TIMSK0, OCIE0A)) {				
				g_tc[a_pin] = 0x00;
				return;
			}
			wait = 1;
			break;

		case PD5_OC0B:
			if (bit_is_clear(TIMSK0, OCIE0B)) {				
				g_tc[a_pin] = 0x00;
				return;
			}
			wait = 1;
			break;
		
		case PB1_OC1A:
			if (bit_is_clear(TIMSK1, OCIE1A)) {				
				g_tc[a_pin] = 0x00;
				return;
			}
			wait = 1;
			break;

		case PB2_OC1B:
			if (bit_is_clear(TIMSK1, OCIE1B)) {				
				g_tc[a_pin] = 0x00;
				return;
			}
			wait = 1;
			break;

		case PB3_OC2A:
			if (bit_is_clear(TIMSK2, OCIE2A)) {				
				g_tc[a_pin] = 0x00;
				return;
			}
			wait = 1;
			break;

		case PD3_OC2B:
			if (bit_is_clear(TIMSK2, OCIE2B)) {				
				g_tc[a_pin] = 0x00;
				return;
			}
			wait = 1;
			break;

		default:
			return;
			break;
	} // switch

	if (wait) while (g_tc[a_pin]);
}


void beeper_off(e_timer_pins a_pin) {
	switch (a_pin) {

		case PD6_OC0A:
		case PD5_OC0B:
			// mask interrupt
			TIMSK0 &= (PD6_OC0A == a_pin ? (~_BV(OCIE0A)) : (~_BV(OCIE0B)));
			TCCR0B = 0x00;
			g_tc[a_pin] = 0x00;
			break;

		case PB1_OC1A:
		case PB2_OC1B:
			// mask interrupt
			TIMSK1 &= (PB1_OC1A == a_pin ? ~_BV(OCIE1A) : ~_BV(OCIE1B));
			TCCR1B &= 0xf8;
			g_tc[a_pin] = 0x00;
			break;

		case PB3_OC2A:
		case PD3_OC2B:
			// mask interrupt
			TIMSK2 &= (PB3_OC2A == a_pin ? ~_BV(OCIE2A) : ~_BV(OCIE2B));
			TCCR2B = 0x00;
			g_tc[a_pin] = 0x00;
			break;

		default:
			return;
			break;
	} // switch
}

/* ================================================================================ */

