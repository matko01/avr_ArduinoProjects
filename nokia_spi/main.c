#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>
#include <math.h>

#include "main.h"
#include "pca.h"



int main(void)
{
	struct dev_pcd8544_ctx lcd;
	struct bus_t spi_bus;
	uint8_t buff = 0;
	uint8_t i = 0;

	spi_bus = spi_hw_poll_bus_get();
	lcd.bus = &spi_bus;

	lcd.sce.port = &PORTB;
	lcd.sce.pin = PORTB0;

	lcd.dc.port = &PORTB;
	lcd.dc.pin = PORTB1;

	lcd.res.port = &PORTB;
	lcd.res.pin = PORTB2;

	spi_hw_poll_init(E_SPI_MODE_MASTER, E_SPI_SPEED_F4);
	pcd8544_init(&lcd);
	pcd8544_clrscr(&lcd);
	pcd8544_install_stdout(&lcd);

	uint8_t c = 0;
	uint8_t yt = 0;
	uint8_t sin_table[256] = {0x00};

	for (uint16_t i = 0; i < 256; i++) {
		sin_table[i] = (cos(i/2)*sin(i))*31 + 32;
	}

	while(1) {
		pcd8544_gotoxy(&lcd, 0, 0);
		for (uint8_t x = 0; x<PCD8544_W; x++) {
			for (uint8_t y = 0; y<PCD8544_H; ) {

				yt = y >> 3;
				
				c = sin_table [ (uint8_t)(i + ((uint8_t)hypot(42 - x, 24 - y) >> 1)) ];

				if (common_ditherPoint(c,x,y)) {
					buff |= _BV( y - (yt << 3) );
				}

				if (!(++y & 0x07)) {
					pcd8544_putblock(&lcd, x, yt, buff);
					buff = 0;
				}
			}
		}
		--i;
	}

	return 0;
}
