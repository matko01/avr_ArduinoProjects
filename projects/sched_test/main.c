#include "serial.h"
#include "sched.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// tick every 1 ms
#define SCHED_TICK_FREQUENCY 100

void task1(void) {
	
	PORTB = 0x00;
	unsigned int x = 0;
	for(x = 0; x < 32768; x++);
	for(x = 0; x < 32768; x++);
	for(x = 0; x < 32768; x++);
	PORTB = 0xff;

}

void task2(void) {
	
	PORTD = 0x00;
	unsigned int x = 0;
	for(x = 0; x < 32768; x++);
	for(x = 0; x < 32768; x++);
	for(x = 0; x < 32768; x++);
	PORTD = 0xff;
}


int main(void) 
{
	serial_init(E_BAUD_115200);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();


	return 0;
}
