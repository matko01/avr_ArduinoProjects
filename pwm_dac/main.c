#include "pca.h"
#include <util/delay.h>
#include <math.h>

#define SINE 0
#define LINE 1

#define CURVE SINE

int main(void)
{
	uint16_t result = 0x00;

	// initialize serial port
	serial_init(E_BAUD_9600);
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();

	// initialize adc single shot
	adc_init(E_SINGLE_SHOT);

	// select internal temperature sensor
	adc_reference_set(E_ADC_EXTERNAL_AVCC);
	adc_channel_set(0);

	// set as outputs
	DDRD = 0xff;
	DDRB = 0xff;

	PORTB = 0x00;
	PORTD = 0x00;

	uint8_t i = 0x00;
	uint8_t sample = 0x00;

	while (1) {

#if CURVE == SINE
		sample = 127.0 + 127.0*sin(2*M_PI*((float)i/255));
#else
		sample = i;
#endif
		i++;

		PORTB = (sample & 0xc0) >> 6;
		PORTD = (sample << 2);
		
		// trigger conversion
		adc_conversion_trigger();

		// get the result
		adc_result_poll_get(result);

		// print it to the serial line
		printf("%d %d\n", result, sample);
		_delay_us(500);
	}


	return 0;
}
