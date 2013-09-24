#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "serial.h"
#include "tdelay.h"

int main(void)
{
	serial_init(E_BAUD_4800);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();

	serial_install_stdio();
	tdelay_init(E_TIMER0);

	while(1) {
		printf("STDIO Serial Hello. Free mem: [%d]\n", common_memory_left());
		/* _delay_ms(1000); */
		tdelay_ms(E_TIMER0, 1000);
	}

	return 0;
}
