#include "lcd.h"
#include "sys_ctx.h"


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


void lcd_blit(uint8_t which) {

	// TODO maybe think about some other method 
	// not to block interrupts so often
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx, 
				(which ? LCD_LINE01_ADDR : LCD_LINE00_ADDR));

		hd44780_puts((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx,
				(char *)g_sys_ctx.display[0]);

		hd44780_goto((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx, 
				(which ? LCD_LINE11_ADDR : LCD_LINE10_ADDR));

		hd44780_puts((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx,
				(char *)g_sys_ctx.display[1]);
	}
}


