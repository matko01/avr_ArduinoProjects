#include "main.h"
#include "sump.h"
#include "common.h"

#include <string.h>


struct trigger {
	uint32_t value;
	uint32_t mask;

	union {
		uint32_t raw;
		struct {
			uint8_t chan4 : 1;
			uint8_t res1 : 1;
			uint8_t serial : 1;
			uint8_t start : 1;
			uint8_t res2 : 4;
			uint8_t level : 2;
			uint8_t res3 : 2;
			uint8_t chan03 : 4;
			uint16_t delay;
		} c;
	} conf;
};

struct settings {
	struct trigger t;
	uint32_t divider;
	uint16_t delay_cnt;
	uint16_t read_cnt;

	union {
		uint8_t raw;
		struct {
			uint8_t demux : 1;
			uint8_t filter : 1;
			uint8_t ch_groups : 4;
			uint8_t external_clk : 1;
			uint8_t invert_clk : 1;
		} bits;
	} flags;
};




void main(void) {

	uint8_t input = 0x00;
	uint8_t data[512];

	// initialize serial port
	serial_init(SERIAL_SPEED);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_install_stdio();
	serial_flush();

	DDRC = 0x00;

	for (;;) {

		if (!serial_getc(&input)) {
			continue;
		}

#if DEBUG_ENABLED == 1
		printf("Received [%02x]\n", input);
		continue;
#endif

		// interpret the command
		switch(input) {

			case SUMP_ID:
				printf("1ALS");
				fflush(stdout);
				break;

			case SUMP_GET_METADATA:
				sump_provide_metadata(DEVICE_PROBES, 
						DEVICE_STATIC_SAMPLES,
						DEVICE_DYNAMIC_SAMPLES,
						DEVICE_SAMPLE_RATE);
				break;

			case SUMP_RUN:
				for (uint8_t i = 0; i < 512; i++) {
					data[i++] = PINC;
				}

				for (uint8_t i = 0; i<128; i++) {
					printf("1234");
				}
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
				break;

			default:
			case SUMP_RESET:
				break;
		}

	}
}
