#include "lcd.h"
#include "fsm.h"

#include <avr/io.h>
#include <util/atomic.h>


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


void lcd_blit(volatile struct lcd_ctx *a_lcd_ctx, uint8_t which) {

	// TODO maybe think about some other method 
	// not to block interrupts so often
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&a_lcd_ctx->dev, (which ? LCD_LINE01_ADDR : LCD_LINE00_ADDR));
		hd44780_puts((struct dev_hd44780_ctx *)&a_lcd_ctx->dev, (char *)a_lcd_ctx->display[0]);
		hd44780_goto((struct dev_hd44780_ctx *)&a_lcd_ctx->dev, (which ? LCD_LINE11_ADDR : LCD_LINE10_ADDR));
		hd44780_puts((struct dev_hd44780_ctx *)&a_lcd_ctx->dev, (char *)a_lcd_ctx->display[1]);
	}
}


__inline__
void lcd_update_backlight(volatile struct lcd_ctx *a_lcd) {
	// fade in / fade out
	if (a_lcd->_lcd_backlight_timer || 0 == a_lcd->settings->lcd_bt_time) {
		if (OCR2A < a_lcd->settings->lcd_brightness) 
			++OCR2A;
	}
	else {
		if (OCR2A) --OCR2A;
	}
}


__inline__
void lcd_update_transition(volatile struct lcd_ctx *a_lcd, volatile struct event_queue *eq) {

	if (a_lcd->_vis_pos != a_lcd->_cur_pos) {

		// screen shift animation
		if (a_lcd->_vis_pos > a_lcd->_cur_pos) {
			if (0 == (a_lcd->_cur_pos++%8))
				// scroll left <--
				hd44780_write((struct dev_hd44780_ctx *)&a_lcd->dev, 
						HD44780_CMD_CD_SHIFT(1, 0), 0);
		}
		else if (a_lcd->_vis_pos < a_lcd->_cur_pos) {
			// scroll right -->
			if (!(a_lcd->_cur_pos--%8))
				hd44780_write((struct dev_hd44780_ctx *)&a_lcd->dev, 
						HD44780_CMD_CD_SHIFT(1, 1), 0);
		}

		if (a_lcd->_vis_pos == a_lcd->_cur_pos)
		{
			// the transition has ended
			fsm_event_push(eq, E_EVENT_TRANSITION_END);
		}
	}
}


void lcd_update_backlight_timer(volatile struct lcd_ctx *a_lcd) {
	// decrement the back light timer
	if (0 < a_lcd->_lcd_backlight_timer) (a_lcd->_lcd_backlight_timer)--;
}


void lcd_clean(volatile struct lcd_ctx *a_lcd, uint8_t a_which) {
	sprintf((char *)a_lcd->display[0],"%16s", " ");
	sprintf((char *)a_lcd->display[1],"%16s", " ");
	lcd_blit(a_lcd, a_which);
}




