#include "sys_settings.h"


void sys_settings_get(struct sys_settings *a_ss) {
	eeprom_read_block(a_ss, SETTINGS_EEPROM_ADDRESS, sizeof(struct sys_settings));

	// initialize if eeprom doesn't contain
	// any settings yet
	if (SETTINGS_MAGIC_ID != a_ss->magic) {
		a_ss->magic = SETTINGS_MAGIC_ID;
		a_ss->lcd_brightness = 0xff;
		a_ss->lcd_contrast = 0xff;
		a_ss->lcd_bt_time = 0;

		a_ss->temp_time = 15;
		a_ss->time_time = 20;
		a_ss->nm_time = 10;
		a_ss->pv_time = 30;

		// initialize
		sys_settings_set(a_ss);
	}
}

