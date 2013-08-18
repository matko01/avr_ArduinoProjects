#include "serial.h"
#include "slip.h"
#include "sched.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>


struct packet {
	uint16_t crc16;
	uint16_t num;
	uint8_t samplerate;
	uint8_t samples[128];
};


void serial_echo(void *a_data __attribute__((unused))) {
	printf("Hello World\r\n");
}


int main(void)
{
	serial_init(E_BAUD_38400);	
	serial_install_interrupts(SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	sched_init();

	sched_task_new(serial_echo, NULL, 200);

	while(1) sched_run();




	/* unsigned char size = 0; */
	/* struct packet pack; */


	/* while (1) { */

	/* 	if (0 >= (size = slip_recv((void *)&pack, sizeof(pack)))) { */
	/* 		continue; */
	/* 	} */

	/* 	// ignore incomplete data chunks */
	/* 	if (size < 6) { */
	/* 		continue; */
	/* 	} */

	/* 	// verify if the information is genuine */
	/* 	if (!slip_verify_crc16((unsigned char *)&pack, size, 0)) { */
	/* 		continue; */
	/* 	} */



	/* 	for (unsigned char i = 0; i < pack.num; i++) { */
	/* 		// play the samples */
	/* 	} */
	/* } */

	return 0;
}
