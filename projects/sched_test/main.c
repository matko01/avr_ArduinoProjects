#include "serial.h"
#include "sched.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// tick every 1 ms
#define SCHED_TICK_FREQUENCY 1000


int main(void)
{
	serial_init(E_BAUD_115200);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	// initialize scheduler
	sched_init(SCHED_TICK_FREQUENCY);

	return 0;
}
