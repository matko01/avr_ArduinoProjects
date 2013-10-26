#include "serial.h"
#include "slip.h"
#include "tpwm.h"
#include "timer_common.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#define PWM_SAMPLES_BUFFER 64
#define PWM_BUFFERS 4

static volatile uint8_t g_head = 0x00, 
			    g_tail = 0x00;


struct packet {
	uint16_t crc16;
	uint8_t num;
	uint8_t samples[PWM_SAMPLES_BUFFER];
};

static volatile struct packet p[PWM_BUFFERS];

static void serial_collect_samples(void *a_data __attribute__((unused))) {

	struct packet *data = (struct packet *)a_data;
	uint8_t size = 0x00;
	uint16_t buffs = (PWM_SAMPLES_BUFFER*PWM_BUFFERS);
	uint16_t avail = (buffs + g_head - g_tail) % buffs;

	if (avail > 192) {
		serial_poll_send("WAIT", 4);
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

	g_head += data->num;

}

static volatile uint8_t cnt = 0x00;


volatile uint8_t sine[] = { 127, 134, 142, 150, 158, 166, 173, 181, 188, 195, 201, 207,
	213, 219, 224, 229, 234, 238, 241, 245, 247, 250, 251, 252, 253, 254, 253,
	252, 251, 250, 247, 245, 241, 238, 234, 229, 224, 219, 213, 207, 201, 195,
	188, 181, 173, 166, 158, 150, 142, 134, 127, 119, 111, 103, 95, 87, 80, 72,
	65, 58, 52, 46, 40, 34, 29, 24, 19, 15, 12, 8, 6, 3, 2, 1, 0, 0, 0, 1, 2,
	3, 6, 8, 12, 15, 19, 24, 29, 34, 40, 46, 52, 58, 65, 72, 80, 87, 95, 103,
	111, 119 };


ISR(TIMER0_COMPA_vect) {
	if (g_tail != g_head) {
		/* OCR1AH = 0x00; */
		/* OCR1AL = p[(g_tail >> 6) & 0x03].samples[g_tail & 0x3f]; */

		volatile uint8_t sample = 0x00;
	   	sample = p[(g_tail >> 6) & 0x03].samples[g_tail & 0x3f];
		PORTD = (sample & 0x3f) << 2;
		PORTB = (sample & 0x03);
		g_tail++;
		/* cnt = cnt == sizeof(sine) ? 0 : (cnt + 1); */

	 	/* sample = sine[cnt]; */
		/* PORTD = sample; */
		/* #<{(| PORTD = (sample << 2) & 0xfc; |)}># */
		/* #<{(| PORTB = (sample & 0x03); |)}># */
		/* cnt++; */

	}
}


int main(void)
{

	serial_init(E_BAUD_115200);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	DDRD = 0xff;
	DDRB = 0xff;

	/* tpwm_fpwm_init(E_TIMER1); */
	/* tpwm_pwm_init(E_TIMER1); */
	/* tpwm_setup_for_bitres(E_TIMER1, 8); */

	sei();
	_timer_init_ctc(E_TIMER0);
	uint32_t pocr = _timer_freq_prescale(E_TIMER0, 4000, 255);
	_timer_setup_ctc(E_TIMER0, pocr);
	_timer_en_compa_int(E_TIMER0);

	while (1) {
		serial_collect_samples((void *)&p[(g_head >> 6) & 0x03]);
	}

	return 0;
}
