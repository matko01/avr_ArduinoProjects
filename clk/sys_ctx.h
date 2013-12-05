#ifndef __SYS_CTX_H__
#define __SYS_CTX_H__

#include <stdint.h>
#include "pca.h"
#include "temperature.h"

/**
 * @brief default magic identifier value
 */
#define SETTINGS_MAGIC_ID 0x66


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

	// back-light on time
	// 0 - always on	
	uint8_t lcd_bt_time;
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
	struct soft_ow sow_ctx;
	struct dev_hd44780_ctx lcd_ctx;
	
	// time
	ds1307_time_t tm;

	// temperature
	volatile struct temp_msr_ctx temp_ctx;

	// system settings
	struct sys_settings settings;

};

#endif /* __SYS_CTX_H__ */
