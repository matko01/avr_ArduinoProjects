#include "pca.h"

#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile static uint8_t cnt = 0x00;
volatile static uint32_t g_epoch = 0x00;

ISR(TIMER0_COMPA_vect) {
	// divide 256 Hz to 1
	if (!cnt) g_epoch++;
	cnt++;
}


int main(void)
{
	uint32_t pocr = 0x00, previous = 0x00;

	// the fastest we can go
	serial_init(E_BAUD_9600);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	// clock tick
	_timer_init_ctc(E_TIMER0);

	// configure timer to generate interrupts with 256 Hz frequency
	pocr = _timer_freq_prescale(E_TIMER0, 128, 255);
	_timer_setup_ctc(E_TIMER0, pocr);
	_timer_en_compa_int(E_TIMER0);

	while (1) {
		if (previous!=g_epoch) {
			previous = g_epoch;
			printf("%02d:%02d:%02d\n", 
					(int)(g_epoch / 3600) % 24,
					(int)(g_epoch / 60) % 60,
					(int)(g_epoch % 60));
		}
		_delay_ms(10);
	}

	return 0;
}
