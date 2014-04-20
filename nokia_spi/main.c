#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "main.h"
#include "pca.h"

#define SERIAL_CE PORTB0
#define DATA_CONTROL PORTB1
#define RESET PORTB2


void lcd_write(uint8_t mode, uint8_t data) {
	if (mode) 
		PORTB |= _BV(DATA_CONTROL);
	else
		PORTB &= ~_BV(DATA_CONTROL);

	PORTB &= ~_BV(SERIAL_CE);



	PORTB |= _BV(SERIAL_CE);
}


int main(void)
{
	uint16_t x = 84*6;

	spi_hw_poll_init(E_SPI_MODE_MASTER, E_SPI_SPEED_F32);
	
	PORTB &= ~_BV(RESET);
	_delay_ms(10);
	PORTB |= _BV(RESET);

	 lcd_write(0, 0x21 );  // LCD Extended Commands.
	 lcd_write(0, 0xB1 );  // Set LCD Vop (Contrast). 
	 lcd_write(0, 0x04 );  // Set Temp coefficent. //0x04
	 lcd_write(0, 0x14 );  // LCD bias mode 1:48. //0x13
	 lcd_write(0, 0x0C );  // LCD in normal mode.
	 lcd_write(0, 0x20 );
	 lcd_write(0, 0x0C );

	while (x--) {
		lcd_write(1, 0xaa);
	}


	while(1);
	return 0;
}
