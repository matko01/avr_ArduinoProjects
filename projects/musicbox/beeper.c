#include "beeper.h"
#include "tdelay.h"

#include <avr/interrupt.h>
#include <avr/power.h>

/* ================================================================================ */

static volatile struct {
	uint32_t duration;
	uint8_t lower_pin;
} g_tc[E_TIMER_LAST] = {0x00};

/* ================================================================================ */

#ifdef TIMER_BEEPER_ISR_2

/**
 * @brief timer 2
 *
 * @param TIMER2_COMPA_vect
 */
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) {
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

#endif



#ifdef TIMER_BEEPER_ISR_1
/**
 * @brief timer 1
 *
 * @param TIMER1_COMPA_vect
 */
ISR(TIMER1_COMPA_vect, ISR_NOBLOCK) {
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

#endif

/* ================================================================================ */

static void _beeper_init_port(volatile uint8_t *port, uint8_t portnum) {
	*port |= _BV(portnum);
}

static void _beeper_init_ocr(volatile uint8_t *ocrh,
		volatile uint8_t *ocrl,
		uint16_t value) {

	if (NULL != ocrh) *ocrh = (value >> 8) & 0xff;
	*ocrl = value & 0xff;
}

static void _beeper_init_tcnt(volatile uint8_t *tcnth,
		volatile uint8_t *tcntl,
		uint16_t value) {

	if (NULL != tcnth) *tcnth = (value >> 8) & 0xff;
	*tcntl = value & 0xff;
}

void beeper_init(e_timer_pins a_pin) {

	// enable interrupts
	sei();

	switch (a_pin) {
		case PD6_OC0A:
			power_timer0_enable();
			_beeper_init_port(&DDRD, PORTD6);
			_beeper_init_ocr(NULL, &OCR0A, 0x00);
			_beeper_init_tcnt(NULL, &TCNT0, 0x00);

			// clock disabled
			TCCR0A = 0x42;
			TCCR0B = 0x00;
			break;

		case PB1_OC1A:
			power_timer1_enable();
			_beeper_init_port(&DDRB, PORTB1);
			_beeper_init_ocr(&OCR1AH, &OCR1AL, 0x00);
			_beeper_init_tcnt(&TCNT1H, &TCNT1L, 0x00);

			// clock disabled, CTC mode
			TCCR1A = 0x40;
			TCCR1B = 0x08;
			break;

		case PB3_OC2A:
			power_timer2_enable();
			_beeper_init_port(&DDRB, PORTB3);
			_beeper_init_ocr(NULL, &OCR2A, 0x00);
			_beeper_init_tcnt(NULL, &TCNT2, 0x00);

			// clock disabled, CTC mode
			TCCR2A = 0x42;
			TCCR2B = 0x00;
			break;

		default:
			break;
	} // switch
}

/**
 * @brief helper function to calculate the presclaer and OCR
 *
 * @param ocr
 * @param prescaler
 * @param a_freq
 * @param criterion
 */
static void _find_ocr_prescaler(uint32_t *ocr, uint8_t *prescaler, uint32_t a_freq, uint32_t criterion) {
	uint8_t prescalers[] = { 0, 3, 6, 8, 10 };
	uint8_t pnum = sizeof(prescalers);
	*prescaler = 0;

	do {
		*ocr = F_CPU / ((a_freq << 1) * (0x01 << prescalers[*prescaler]));
	} while ((*ocr > criterion) && (++(*prescaler) < pnum));
}

/**
 * @brief calculate prescaler and OCR value for 8 bit counters
 *
 * @param a_freq requested frequency
 * @param a_ocr ocr value returned by reference
 * @param a_prescaler prescaler value returned by reference
 */
static void _timer8_prescaler(uint32_t a_freq, 
		uint8_t *a_ocr, 
		uint8_t *a_prescaler) {

	uint32_t ocr = 0x00;

	// smallest achievable frequency with this timer
	if (a_freq < 62) {
		*a_prescaler = 5;
		*a_ocr = 0;
		return;
	}

	_find_ocr_prescaler(&ocr, a_prescaler, a_freq, 255);
	*a_ocr = (ocr & 0xff);	
}

/**
 * @brief calculate prescaler and ocr values for 16 bit counter
 *
 * @param a_freq requested frequency
 * @param a_ocrh ocr high
 * @param a_ocrl ocr low
 * @param a_prescaler prescaler value
 */
static void _timer16_prescaler(uint32_t a_freq,
		uint8_t *a_ocrh,
		uint8_t *a_ocrl,
		uint8_t *a_prescaler) {

	uint32_t ocr = 0x00;

	_find_ocr_prescaler(&ocr, a_prescaler, a_freq, 65535);
	*a_ocrh = (ocr & 0xff00) >> 8;
	*a_ocrl = (ocr & 0xff);
}

static void _beep_timer8(e_timer_pins pin, 
		uint32_t freq,
		uint32_t duration,
		volatile uint8_t *tcca,
		volatile uint8_t *tccb,
		volatile uint8_t *ocra) {


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
			TCCR0B &= 0xf8;

			if (freq) {
				TCCR0A |= 0x40;
				_timer8_prescaler(freq, &ocr, &presc);

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

			OCR0A = ocr;
			TIMSK0 |= _BV(OCIE0A);
			break;

		case PB1_OC1A:
			TCCR1B &= 0xf8;
			if (freq) {
				_timer16_prescaler(freq, &ocrh, &ocr, &presc);
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

			OCR1AH = ocrh;
			OCR1AL = ocr;
			TIMSK1 |= _BV(OCIE1A);
			break;

		case PB3_OC2A:
			TCCR2B &= 0xf8;
			if (freq) {
				_timer8_prescaler(freq, &ocr, &presc);
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

			OCR2A = ocr;
			TIMSK2 |= _BV(OCIE2A);
			break;

		default:
			return;
			break;
	} // switch
}


static uint8_t _beeper_block(e_timer_pins a_pin, volatile uint8_t *timsk, uint8_t map) {
	if (bit_is_clear(*timsk, map)) {				
		g_tc[a_pin] = 0x00;
		return 0;
	}
	return 1;
}

void beeper_block(e_timer_pins a_pin) {
	unsigned char wait = 0;

	// deadlock protection
	switch (a_pin) {
		case PD6_OC0A:
			wait = _beeper_block(a_pin, &TIMSK0, OCIE0A);
			break;

		case PB1_OC1A:
			wait = _beeper_block(a_pin, &TIMSK1, OCIE1A);
			break;

		case PB3_OC2A:
			wait = _beeper_block(a_pin, &TIMSK2, OCIE2A);
			break;

		default:
			return;
			break;
	} // switch

	if (wait) while (g_tc[a_pin]);
}


static void _beeper_off(volatile uint8_t *timsk, 
		volatile uint8_t *tccrb,
		uint8_t mask,
		uint8_t tcc) {

	// mask interrupt
	*timsk &= mask;
	*tccrb &= tcc;
}

void beeper_off(e_timer_pins a_pin) {
	switch (a_pin) {

		case PD6_OC0A:
			_beeper_off(&TIMSK0, &TCCR0B, ~_BV(OCIE0A), 0x00);
			break;

		case PB1_OC1A:
			_beeper_off(&TIMSK1, &TCCR1B, ~_BV(OCIE1A), 0xf8);
			break;

		case PB3_OC2A:
			_beeper_off(&TIMSK2, &TCCR2B,  ~_BV(OCIE2A), 0x00);
			break;

		default:
			break;
	} // switch

	g_tc[a_pin] = 0x00;
}

/* ================================================================================ */

