#include "serial.h"
#include "aos.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// tick every 1 ms
#define SCHED_TICK_FREQUENCY 100

void task1(void) {
	
	PORTB = 0x00;
	_delay_ms(500);

	PORTB = 0xff;
	_delay_ms(500);

}

void task2(void) {
	
	PORTD = 0x00;
	_delay_ms(500);

	PORTD = 0xff;
	_delay_ms(500);
}


int main(void) 
{
	serial_init(E_BAUD_115200);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	aos_init(SCHED_TICK_FREQUENCY);

	return 0;
}
