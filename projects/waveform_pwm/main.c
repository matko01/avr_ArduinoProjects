#include "serial.h"
#include "slip.h"
#include "sched.h"
#include "tpwm.h"
#include "timer_common.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// 1kB buffer should suffice for around 100ms at 64kbps
#define PWM_SAMPLES_BUFFER 1024

static unsigned char g_samples[PWM_SAMPLES_BUFFER] = {0x00};
static volatile uint16_t g_head = 0x00, 
			    g_tail = 0x00;

struct packet {
	uint16_t crc16;
	uint8_t num;
	uint8_t samples[32];
};


static void serial_collect_samples(void *a_data __attribute__((unused))) {
	
	struct packet *data = (struct packet *)a_data;
	unsigned char size = 0x00;

	if (0 >= (size = slip_recv((void *)data, 
					sizeof(struct packet)))) {
		return;
	}

	// ignore incomplete data chunks
	if (size < 5) {
		return;
	}

	// verify if the information is genuine
	if (!slip_verify_crc16((unsigned char *)data, size, 0)) {
		return;
	}

	unsigned char i = 0x00;
	uint16_t next = (g_head + 1) % PWM_SAMPLES_BUFFER;
	
	while (data->num-- && next != g_tail) {
		next = (g_head + 1) % PWM_SAMPLES_BUFFER;
		g_samples[g_head] = data->samples[i++];
		g_head = next;
	}
}


ISR(TIMER0_COMPA_vect) {
	if (g_tail != g_head) {
		OCR1AH = 0x00;
		OCR1AL = g_samples[g_tail];
		g_tail = (g_tail + 1) % PWM_SAMPLES_BUFFER;
	}
}


int main(void)
{
	struct packet pack;

	serial_init(E_BAUD_115200);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	tpwm_pwm_init(E_TIMER1);
	/* tpwm_setup_for_bitres(E_TIMER1, 8); */
	tpwm_setup_for_freq(E_TIMER1, 60000);

	_timer_init_ctc(E_TIMER0);
	uint32_t pocr = _timer_freq_prescale(E_TIMER0, 4000, 255);
	_timer_setup_ctc(E_TIMER0, pocr);
	_timer_en_compa_int(E_TIMER0);

	while (1) {
		serial_collect_samples((void *)&pack);
	}

	return 0;
}
