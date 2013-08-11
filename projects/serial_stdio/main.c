#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "serial.h"

int main(void)
{
	serial_init(E_BAUD_4800);	
	serial_install_interrupts();
	serial_flush();

	serial_install_stdio();

	while(1) {
		printf("STDIO Serial Hello \n");
		_delay_ms(1000);
	}

	return 0;
}
