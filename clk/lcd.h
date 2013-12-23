#ifndef __LCD_H__
#define __LCD_H__

#include "pca.h"
#include "sys_config.h"
#include "sys_settings.h"
#include "fsm.h"


#define LCD_CHARACTERS_PER_LINE 16
#define LCD_NUMBER_OF_LINES 2


#define LCD_LINE00_ADDR 0x00
#define LCD_LINE01_ADDR 0x11
#define LCD_LINE10_ADDR 0x40
#define LCD_LINE11_ADDR 0x51


struct lcd_ctx {

	// lcd timeout
	volatile uint16_t _lcd_backlight_timer;

	// visible position defines the screen offset
	// currently displayed (0 or 17 most of the times)
	volatile uint8_t _vis_pos;

	// current position is used during the transition
	// to define the screen frame position
	volatile uint8_t _cur_pos;

	// display buffer
	char display[LCD_NUMBER_OF_LINES][LCD_CHARACTERS_PER_LINE + 1];

	// device structure
	struct dev_hd44780_ctx dev;

	// settings pointer
	struct sys_settings *settings;
};


void lcd_setup(struct dev_hd44780_ctx *a_lcd_ctx);
void lcd_blit(struct lcd_ctx *a_lcd_ctx, uint8_t which);

void lcd_update_backlight(volatile struct lcd_ctx *a_lcd_ctx);
void lcd_update_transition(volatile struct lcd_ctx *a_lcd, volatile struct event_queue *eq);
void lcd_update_backlight_timer(volatile struct lcd_ctx *a_lcd);


void lcd_clean(struct lcd_ctx *a_lcd, uint8_t a_which);


#endif /* __LCD_H__ */
