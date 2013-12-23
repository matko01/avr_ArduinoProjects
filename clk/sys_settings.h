#ifndef __SYS_SETTINGS_H__
#define __SYS_SETTINGS_H__

#include <avr/eeprom.h>
#include <stdint.h>


/**
 * @brief default magic identifier values
 */
#define SETTINGS_MAGIC_ID 0x0d


/**
 * @brief address of the structure in the EEPROM memory
 */
#define SETTINGS_EEPROM_ADDRESS (void *)0x00


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
	uint16_t lcd_bt_time;

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
 * @brief restore system settings from EEPROM
 *
 * @param a_ss system settings structure
 */
void sys_settings_get(struct sys_settings *a_ss);


#define sys_settings_set(__ss) \
		eeprom_write_block(__ss, SETTINGS_EEPROM_ADDRESS, sizeof(struct sys_settings));


#endif /* __SYS_SETTINGS_H__ */
