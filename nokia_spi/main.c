#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <util/delay.h>

#include "main.h"
#include "pca.h"



int main(void)
{
	struct dev_pcd8544_ctx lcd;
	uint8_t buff[(PCD8544_W * PCD8544_H) >> 3] = {0x00};
	uint8_t i = 0;

	lcd.bus = &g_bus_spi_hw_poll;

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

	while(1) {
		memset(buff, 0x00, sizeof(buff));
		pcd8544_gotoxy(&lcd, 0, 0);
		for (uint8_t x = 0; x<PCD8544_W; x++) {
			for (uint8_t y = 0; y<PCD8544_H; ) {
				if (!((x + i) & y)) PCD8544_SET_BIT_HIGH(buff, x, y);
				y++;

				if (!(y%8))
				pcd8544_putpixel(&lcd, x, (y >> 3) - 1, buff[x + ((y >> 3) - 1)*84]);
			}
		}
		i++;
		_delay_ms(20);
	}

	return 0;
}
