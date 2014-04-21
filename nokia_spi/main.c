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
	uint16_t x = 84*6;

	lcd.bus = &g_bus_spi_hw_poll;

	lcd.sce.port = &PORTB;
	lcd.sce.pin = PORTB0;

	lcd.dc.port = &PORTB;
	lcd.dc.pin = PORTB1;

	lcd.res.port = &PORTB;
	lcd.res.pin = PORTB2;

	spi_hw_poll_init(E_SPI_MODE_MASTER, E_SPI_SPEED_F8);
	pcd8544_init(&lcd);
	pcd8544_clrscr(&lcd);

	pcd8544_write(&lcd, PCD8544_CMD, PCD8544_CMD_SET_X(0));
	pcd8544_write(&lcd, PCD8544_CMD, PCD8544_CMD_SET_Y(0));

	pcd8544_putc(&lcd, 'T');
	pcd8544_putc(&lcd, 'o');
	pcd8544_putc(&lcd, 'm');
	pcd8544_putc(&lcd, 'e');
	pcd8544_putc(&lcd, 'k');
	/* while (x--) { */
	/* 	pcd8544_write(&lcd, PCD8544_DATA, 0xaa); */
	/* } */

	while(1);
	return 0;
}
