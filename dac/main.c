#include "serial.h"
#include "slip.h"
#include "tpwm.h"
#include "timer_common.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


#define MODE_8K_8B 0
#define MODE_8K_16B 1
#define MODE_16K_8B 2


/// redefine this to any of the values above to change the mode
#define MODE MODE_8K_16B




#if MODE == MODE_8K_16B
#define PWM_SAMPLES_BUFFER 64
#else
#define PWM_SAMPLES_BUFFER 128
#endif

#define PWM_BUFFERS 4


#if MODE == MODE_8K_16B
static volatile uint8_t
#else
static volatile uint16_t 
#endif

g_head = 0x00, 
g_tail = 0x00;

/**
 * @brief in order to minimize runtime calculations
 * precompute how many samples can be stored in buffers in overall
 */
static volatile uint16_t max_samples = 0x00;

struct packet {
	uint16_t crc16;
	uint8_t num;

#if MODE == MODE_8K_16B
	int16_t samples[PWM_SAMPLES_BUFFER];
#else
	uint8_t samples[PWM_SAMPLES_BUFFER];
#endif

};

static volatile struct packet p[PWM_BUFFERS];

static void serial_collect_samples(void *a_data __attribute__((unused))) {

	struct packet *data = (struct packet *)a_data;
	uint8_t size = 0x00;
	uint16_t avail = (max_samples + g_head - g_tail) % max_samples;

#if MODE == MODE_8K_16B
	if (avail >= 192) {
#else
	if (avail >= 384) {
#endif
		serial_poll_send("W", 1);
	}

	if (0 >= (size = slip_recv((void *)data, 
					sizeof(struct packet)))) {
		return;
	}

	// ignore incomplete data chunks, packet should contain at least 1 sample
	if (size < 4) {
		return;
	}

	// verify if the information is genuine
	if (!slip_verify_crc16((unsigned char *)data, size, 0)) {
		return;
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		g_head = (g_head + data->num) % max_samples;
	}

}


ISR(TIMER0_COMPA_vect, ISR_NOBLOCK) {
	if (g_tail != g_head) {
		OCR1AH = 0x00;
		OCR1BH = 0x00;

#if MODE == MODE_8K_16B
		uint16_t sample = (0x8000 + (p[(g_tail >> 6)].samples[g_tail & 0x3f]));
		OCR1AL = sample >> 8 ;
		OCR1BL = (sample) & 0xff;
#else
		OCR1AL = p[(g_tail >> 7)].samples[g_tail & 0x7f];
#endif

		g_tail = (g_tail + 1) % max_samples;
	}
}


int main(void)
{
	// the fastest we can go
	serial_init(E_BAUD_230400);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();

	max_samples = PWM_SAMPLES_BUFFER * PWM_BUFFERS;

	/* tpwm_fpwm_init(E_TIMER1); */

#if MODE == MODE_8K_16B
	tpwm_pwm_init(E_TIMER1, E_PWM_DOUBLE);
#else
	tpwm_pwm_init(E_TIMER1, E_PWM_SINGLE);
#endif

	tpwm_setup_for_bitres(E_TIMER1, 8);

	// the source to generate interrupts to consume 
	// the samples
	_timer_init_ctc(E_TIMER0);

#if MODE == MODE_16K_8B
	uint32_t pocr = _timer_freq_prescale(E_TIMER0, 8000, 255);
#else
	uint32_t pocr = _timer_freq_prescale(E_TIMER0, 4000, 255);
#endif

	_timer_setup_ctc(E_TIMER0, pocr);
	_timer_en_compa_int(E_TIMER0);

	while (1) {
#if MODE == MODE_8K_16B
		serial_collect_samples((void *)&p[(g_head >> 6)]);
#else
		serial_collect_samples((void *)&p[(g_head >> 7)]);
#endif
	}

	return 0;
}
