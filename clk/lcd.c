#include "lcd.h"


void lcd_setup(struct dev_hd44780_ctx *a_lcd_ctx) {
	
	// lcd setup
	a_lcd_ctx->rs.port = &LCD_RS_PORT;
	a_lcd_ctx->rs.pin = LCD_RS_PIN;
	a_lcd_ctx->e.port = &LCD_E_PORT;
	a_lcd_ctx->e.pin = LCD_E_PIN;

	// setup data lines
	for (uint8_t x = 0; x<4; x++) {
		a_lcd_ctx->data[x].port = &LCD_DB_PORT;
		a_lcd_ctx->data[x].pin = LCD_DB_PIN_FIRST + x;
	}

	// display specifics
	a_lcd_ctx->lines = LCD_NUMBER_OF_LINES;
	a_lcd_ctx->font = HD44780_FONT_5X8;

	// initialize the device
	hd44780_init(a_lcd_ctx);
}

