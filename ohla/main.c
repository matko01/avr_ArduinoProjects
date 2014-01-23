#include "main.h"
#include "sump.h"

#include <string.h>


#define SERIAL_SPEED E_BAUD_115200


void main(void) {

	uint8_t input = 0x00;
	uint8_t long_cmd[5] = {0x00};
	uint8_t idx = 0x00;

	// initialize serial port
	serial_init(SERIAL_SPEED);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();
	
	for (;;) {

		if (!serial_getc(&input)) {
			continue;
		}

#ifdef DEBUG_ENABLED
		printf("Received [%02x]\n", input);
		continue;
#endif

		if (long_cmd[0]) {

			// collect the data
			if (idx < 5) {
				long_cmd[idx++] = input;
				continue;
			}

			// long command data has been collected and 
			// can be interpreted
			switch(long_cmd[0]) {

				case SUMP_SET_TRIGGER_MASK:
					break;

				case SUMP_SET_TRIGGER_VALUE:
					break;

				case SUMP_SET_TRIGGER_CONF:
					break;

				case SUMP_SET_DIVIDER:
					break;

				case SUMP_SET_READY_DELAY_COUNT:
					break;

				case SUMP_SET_FLAGS:
					break;
			}

			// wipe it out
			memset(long_cmd, 0x00, sizeof(long_cmd));
			continue;
		}


		// interpret the command
		switch(input) {

			case SUMP_ID:
				printf("SLA0");
				break;

			case SUMP_RUN:
				break;

			case SUMP_XON:
				break;

			case SUMP_XOFF:
				break;

			case SUMP_SET_TRIGGER_MASK:
			case SUMP_SET_TRIGGER_VALUE:
			case SUMP_SET_TRIGGER_CONF:
			case SUMP_SET_DIVIDER:
			case SUMP_SET_READY_DELAY_COUNT:
			case SUMP_SET_FLAGS:
				idx = 0;
				long_cmd[idx++] = input;
				break;

			default:
			case SUMP_RESET:
				break;
		}

	}
}
