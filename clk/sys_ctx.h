#ifndef __SYS_CTX_H__
#define __SYS_CTX_H__

#include <stdint.h>
#include "pca.h"
#include "temperature.h"
#include "fsm.h"
#include "lcd.h"

/**
 * @brief default magic identifier value
 */
#define SETTINGS_MAGIC_ID 0x0b


/**
 * @brief permanent settings
 */
struct sys_settings {

	// settings magic identifier
	// if presents - it means that the clock has been initialized
	uint8_t magic;

	// lcd brightness
	// 0 - disabled
	uint8_t lcd_brightness;

	// contrast of the LCD
	uint8_t lcd_contrast;

	// back-light on time
	// 0 - always on	
	uint8_t lcd_bt_time;

	// how long the temp screen will be displayed
	uint8_t temp_time;

	// how long the time screen will be displayed
	uint8_t time_time;

	// how long the name-day screen will be displayed
	uint8_t nm_time;

	// how long the proverb screen will be displayed
	uint8_t pv_time;
};


/**
 * @brief buttons enumeration
 */
enum e_buttons {
	E_MENU = 0,
	E_MINUS,
	E_PLUS,
	E_OK,

	E_BUTTON_LAST
};


/**
 * @brief system context
 *
 * Contains all system variables
 *
 */
struct sys_ctx {
	
	// buses
	volatile struct twi_ctx *twi_ctx;
	volatile struct soft_ow sow_ctx;
	struct dev_hd44780_ctx lcd_ctx;

	// LED
	gpio_pin led;
	
	// time
	ds1307_time_t tm;

	// temperature
	volatile struct temp_msr_ctx temp_ctx;

	// system settings
	struct sys_settings settings;

	// lcd timeout
	volatile uint16_t lcd_backlight_timer;

	// each bit represents the button state
	volatile uint8_t buttons;

	// main state machine
	struct fsm_t fsm;
	struct event_queue eq;
	f_state_cb state_cb[E_INVALID];

	// ---- work variables bellow ---- 

	// this one is triggered every second to
	// inhibit update of the displayed time
	volatile uint8_t _time_trigger;

	// this is a rolling low-precision counter
	// incremented by the hardware timer every ~16 ms
	// used for text scrolling and animations
	volatile uint16_t _fast_counter;

	// event timer is set by various routines
	// and decrement every second
	// when reaches 0 an EVENT_TO will be queued
	volatile uint8_t _event_timer;

	// visible position defines the screen offset
	// currently displayed (0 or 17 most of the times)
	volatile uint8_t _vis_pos;

	// current position is used during the transition
	// to define the screen frame position
	volatile uint8_t _cur_pos;

	// display buffer
	char display[LCD_NUMBER_OF_LINES][LCD_CHARACTERS_PER_LINE + 1];
};

#endif /* __SYS_CTX_H__ */
