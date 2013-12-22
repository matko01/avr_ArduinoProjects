#include "menu.h"
#include "pca.h"
#include "fsm.h"
#include "lcd.h"
#include "main.h"
#include "sys_ctx.h"
#include "sys_common.h"
#include "scroll_string.h"

#include <string.h>
#include <util/delay.h>
#include <avr/eeprom.h>

/* ================================================================================ */

static void menu_set_time(uint8_t);
static void menu_set_date(uint8_t);
static void menu_set_time_mode(uint8_t);
static void menu_set_lcd_brightness(uint8_t);
static void menu_set_lcd_contrast(uint8_t);
static void menu_set_lcd_backlight(uint8_t);
static void menu_reset_temperature(uint8_t);
static void menu_set_temp_disp_time(uint8_t);
static void menu_set_time_disp_time(uint8_t);
static void menu_set_nameday_disp_time(uint8_t);
static void menu_set_wow_disp_time(uint8_t);
static void menu_save_settings(uint8_t);

/* ================================================================================ */

struct menu_item items[] = {
	{ "Set Time", 					MENU_ITEM_DEFAULT, { menu_set_time } }, 
	{ "Set Date", 					MENU_ITEM_DEFAULT, { menu_set_date } }, 
	{ "Time Mode", 					MENU_ITEM_DEFAULT, { menu_set_time_mode } }, 
	{ "LCD Brightness", 			MENU_ITEM_DEFAULT, { menu_set_lcd_brightness } }, 
	{ "LCD Contrast", 				MENU_ITEM_DEFAULT, { menu_set_lcd_contrast } },
	{ "LCD Backlight Time", 		MENU_ITEM_DEFAULT, { menu_set_lcd_backlight } }, 
	{ "Reset temperature", 			MENU_ITEM_DEFAULT, { menu_reset_temperature } },
	{ "Temperature Display Time", 	MENU_ITEM_DEFAULT, { menu_set_temp_disp_time } },
	{ "Time Display Time", 			MENU_ITEM_DEFAULT, { menu_set_time_disp_time } },
	{ "Nameday Display Time", 		MENU_ITEM_DEFAULT, { menu_set_nameday_disp_time } },
	{ "WoW Display Time", 			MENU_ITEM_DEFAULT, { menu_set_wow_disp_time } },
	{ "Save Settings to EEPROM", 	MENU_ITEM_DEFAULT, { menu_save_settings } }
};


struct menu g_main_menu = {
	.items = items,
	.parent = NULL,
	.cnt = MENU_ITEMS_SIZE(items),
	._cursor = 0,
	._is = NULL
};

/* ================================================================================ */

void menu_render_progress_bar(char *a_buffer, uint8_t a_len, uint8_t a_min, uint8_t a_max, uint8_t a_cur) {
	uint8_t range = a_max - a_min;
	uint8_t pos = a_cur - a_min;
	uint8_t progress = (uint8_t)(((float)pos/range)*a_len);

	if (a_len>3) {
		*a_buffer = '[';
		a_buffer[a_len - 1] = ']';
	}

	for (uint8_t i = 1; i<a_len-1; i++) {
		a_buffer[i] = i<progress ? '#' : ' ';
	}
}


void menu_render(struct menu *a_menu) {
	uint8_t len = 0;
	uint8_t so = (a_menu->_cursor & 0xfe);
	static struct scroll_str str = {0x00};
	char output[LCD_CHARACTERS_PER_LINE + 1] = {0x00};

	if (NULL != a_menu->_is && 
			!(a_menu->_is->config & MENU_ITEM_INACTIVE)) {

		if (str.s != a_menu->_is->name) str.pos = 0;
		str.s = (char *)a_menu->_is->name;
		str.len = strlen(a_menu->_is->name);

		// get a string slice
		scroll_str_paste(&str, output, sizeof(output) - 1, g_sys_ctx._fast_counter);

		// render the function name
		snprintf((char *)g_sys_ctx.display[0], 
				LCD_CHARACTERS_PER_LINE + 1, 
				"%-16s",				
			   	output);

		// ... and the function itself
		if (!(a_menu->_is->config & MENU_ITEM_SUBMENU)) 
			a_menu->_is->ptr.cb(E_EVENT_NONE);

		return;
	}

	// render items
	for (uint8_t i = 0; i<2; i++) {
		const char *istr = a_menu->items[(i + so) % a_menu->cnt].name;

		// clear the buffer
		len = LCD_CHARACTERS_PER_LINE + 1;
		common_zero_mem(g_sys_ctx.display[i], len);
		
		if (i + so == a_menu->_cursor) {
			if (str.s != istr) str.pos = 0;
			str.s = (char *)istr;
			str.len = strlen(istr);

			// get a string slice
			scroll_str_paste(&str, output, sizeof(output) - 1, g_sys_ctx._fast_counter);
		}

		// render an item
		snprintf((char *)g_sys_ctx.display[i], 
				LCD_CHARACTERS_PER_LINE + 1, 
				"%c%-15s",
				i + so == a_menu->_cursor ? '>' : ' ',
			   	i + so == a_menu->_cursor ? output : istr);
	}
}


void menu_process_input(struct menu *a_menu, uint8_t a_input) {
	switch (a_input) {
		case E_EVENT_BUTTON_PLUS:
			if (NULL == a_menu->_is) {
				a_menu->_cursor = 
					(a_menu->_cursor + 1) % a_menu->cnt;
			}
			else {
				a_menu->_is->ptr.cb(a_input);
			}
			break;

		case E_EVENT_BUTTON_MINUS:
			if (NULL == a_menu->_is) {
				a_menu->_cursor = a_menu->_cursor ?
				   (a_menu->_cursor - 1) : (a_menu->cnt - 1);
			}
			else {
				a_menu->_is->ptr.cb(a_input);
			}			
			break;

		case E_EVENT_BUTTON_OK:
			if (NULL == a_menu->_is) {
				a_menu->_is = &a_menu->items[a_menu->_cursor];
			}
			else {
				a_menu->_is->ptr.cb(a_input);
				a_menu->_is = NULL;
			}
			break;
	} // switch
	_delay_ms(300);
}


static void menu_set_time(uint8_t a_event) {
	// TODO implement me
}


static void menu_set_date(uint8_t a_event) {
	// TODO implement me
}


static void menu_reset_temperature(uint8_t a_event) {
	// TODO implement me
}


static void menu_set_time_mode(uint8_t a_event) {
	uint8_t data[2] = {DS1307_HOURS_ADDR};
	const char *mode[] = {
		"24 mode",
		"12 AM/PM mode"
	};

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
		case E_EVENT_BUTTON_PLUS:

			twi_mtx(TWI_RTC_ADDR, data, 0x01, 0x00);
			while (g_sys_ctx.twi_ctx->status & E_TWI_BIT_BUSY);
			twi_mrx(TWI_RTC_ADDR, &data[1], sizeof(uint8_t), E_TWI_BIT_SEND_STOP);
			while (g_sys_ctx.twi_ctx->status & E_TWI_BIT_BUSY);

			if (data[1] & 0x40) {
				data[1] &= ~0x40;
			}
			else {
				data[1] |= 0x40;
			}

			twi_mtx(TWI_RTC_ADDR, data, 2, E_TWI_BIT_SEND_STOP);
			while (g_sys_ctx.twi_ctx->status & E_TWI_BIT_BUSY);
			break;
	} // switch

	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			" %-15s",
			mode[(g_sys_ctx.tm.mode_ampm_hour & 0x40) >> 6]);
}


static void menu_set_lcd_brightness(uint8_t a_event) {
	char pg[13] = {0x00};

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (g_sys_ctx.settings.lcd_brightness >= 0x08)
				g_sys_ctx.settings.lcd_brightness -= 0x08;
			else 
				g_sys_ctx.settings.lcd_brightness = 0;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (g_sys_ctx.settings.lcd_brightness < 0xf7)
				g_sys_ctx.settings.lcd_brightness += 0x08;
			else 
				g_sys_ctx.settings.lcd_brightness = 0xff;
			break;
	} // switch

	SET_BRIGHTNESS(g_sys_ctx.settings.lcd_brightness);
	menu_render_progress_bar(pg, 12, 0, 255, g_sys_ctx.settings.lcd_brightness);

	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"%03d %s",
			g_sys_ctx.settings.lcd_brightness,
			pg);
}


static void menu_set_lcd_contrast(uint8_t a_event) {
	char pg[13] = {0x00};

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (g_sys_ctx.settings.lcd_contrast >= 0x08)
				g_sys_ctx.settings.lcd_contrast -= 0x08;
			else 
				g_sys_ctx.settings.lcd_contrast = 0;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (g_sys_ctx.settings.lcd_contrast < 0xf7)
				g_sys_ctx.settings.lcd_contrast += 0x08;
			else 
				g_sys_ctx.settings.lcd_contrast = 0xff;
			break;
	} // switch

	SET_CONTRAST(g_sys_ctx.settings.lcd_contrast);
	menu_render_progress_bar(pg, 12, 0, 255, g_sys_ctx.settings.lcd_contrast);

	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"%03d %s",
			g_sys_ctx.settings.lcd_contrast,
			pg);
}


static void menu_set_lcd_backlight(uint8_t a_event) {
	const char *backlight[] = {
		"Always On",
		"10 secs",
		"30 secs",
		"60 secs",
		"5 mins",
		"15 mins",
		"30 mins",
		"60 mins"
	};

	uint16_t times[] = {
		0,
		10,
		30,
		60,
		(5*60),
		(15*60),
		(30*60),
		(60*60)
	};

	static int8_t idx = -1;
	uint8_t mc = sizeof(times)/sizeof(times[0]);
	
	// perform initialization
	if (-1 == idx) {
		idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < mc; i++) {
			if (g_sys_ctx.settings.lcd_bt_time == times[i]) {
				idx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (idx) idx--;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (idx<(mc-1)) idx++;
			break;
	} // switch

	g_sys_ctx.settings.lcd_bt_time = times[idx];
	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %-14s",
			backlight[idx]);
}


static void menu_set_temp_disp_time(uint8_t a_event) {
	uint8_t times[] = { 10, 15, 20, 30, 60, 90 };
	static int8_t idx = -1;
	
	// perform initialization
	if (-1 == idx) {
		idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < sizeof(times); i++) {
			if (g_sys_ctx.settings.temp_time == times[i]) {
				idx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (idx) idx--;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (idx<(sizeof(times)-1)) idx++;
			break;
	} // switch

	g_sys_ctx.settings.temp_time = times[idx];
	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %2d secs       ",
			times[idx]);
}


static void menu_set_time_disp_time(uint8_t a_event) {
	uint8_t times[] = { 10, 15, 20, 30, 60, 90 };
	static int8_t idx = -1;
	
	// perform initialization
	if (-1 == idx) {
		idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < sizeof(times); i++) {
			if (g_sys_ctx.settings.time_time == times[i]) {
				idx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (idx) idx--;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (idx<(sizeof(times) - 1)) idx++;
			break;
	} // switch

	g_sys_ctx.settings.time_time = times[idx];
	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %2d secs       ",
			times[idx]);

}


static void menu_set_nameday_disp_time(uint8_t a_event) {
	uint8_t times[] = { 10, 15, 20, 30, 60, 90 };
	static int8_t idx = -1;
	
	// perform initialization
	if (-1 == idx) {
		idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < sizeof(times); i++) {
			if (g_sys_ctx.settings.nm_time == times[i]) {
				idx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (idx) idx--;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (idx<(sizeof(times) - 1)) idx++;
			break;
	} // switch

	g_sys_ctx.settings.nm_time = times[idx];
	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %2d secs       ",
			times[idx]);
}


static void menu_set_wow_disp_time(uint8_t a_event) {
	uint8_t times[] = { 10, 15, 20, 30, 60, 90 };
	static int8_t idx = -1;
	
	// perform initialization
	if (-1 == idx) {
		idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < sizeof(times); i++) {
			if (g_sys_ctx.settings.pv_time == times[i]) {
				idx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (idx) idx--;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (idx<(sizeof(times) - 1)) idx++;
			break;
	} // switch

	g_sys_ctx.settings.pv_time = times[idx];
	snprintf((char *)g_sys_ctx.display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %2d secs       ",
			times[idx]);
}


static void menu_save_settings(uint8_t a_event) {
	static uint8_t cnt = 30;

	if (E_EVENT_BUTTON_OK == a_event) {
		// write the settings structure back to EEPROM
		eeprom_write_block((void *)&g_sys_ctx.settings, 
				(void *)0x00, sizeof(struct sys_settings));
		return;
	}

	sprintf((char *)g_sys_ctx.display[1], " SETTINGS SAVED ");
	_delay_ms(100);
	cnt--;

	if (!cnt--) {
		// press OK :)
		cnt = 30;
		fsm_event_push(&g_sys_ctx.eq, E_EVENT_BUTTON_OK);
	}
}

