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
#define SETTINGS_MAGIC_ID 0x67


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

	// main state machine
	struct fsm_ctx fsm;

	// working variables
	volatile uint8_t _time_trigger;
	volatile uint8_t _vis_pos;
	volatile uint8_t _cur_pos;

	// display buffer
	char display[LCD_NUMBER_OF_LINES][LCD_CHARACTERS_PER_LINE + 1];
};

#endif /* __SYS_CTX_H__ */
