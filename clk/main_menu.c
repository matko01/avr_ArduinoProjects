#include "main_menu.h"
#include "menu.h"
#include "fsm_private_data.h"
#include "sys_setup.h"

/* ================================================================================ */

static void menu_set_time(void*,uint8_t);
static void menu_set_date(void*,uint8_t);
static void menu_set_time_mode(void*,uint8_t);
static void menu_set_lcd_brightness(void*,uint8_t);
static void menu_set_lcd_contrast(void*,uint8_t);
static void menu_set_lcd_backlight(void*,uint8_t);
static void menu_reset_temperature(void*,uint8_t);
static void menu_set_temp_disp_time(void*,uint8_t);
static void menu_set_time_disp_time(void*,uint8_t);
static void menu_set_nameday_disp_time(void*,uint8_t);
static void menu_set_wow_disp_time(void*,uint8_t);
static void menu_save_settings(void*,uint8_t);

/* ================================================================================ */

static struct menu_item items[] = {
	{ "Set Time", 						MENU_ITEM_DEFAULT, { menu_set_time } }, 
	{ "Set Date", 						MENU_ITEM_DEFAULT, { menu_set_date } }, 
	{ "Time Mode", 						MENU_ITEM_DEFAULT, { menu_set_time_mode } }, 
	{ "LCD Brightness", 				MENU_ITEM_DEFAULT, { menu_set_lcd_brightness } }, 
	{ "LCD Contrast", 					MENU_ITEM_DEFAULT, { menu_set_lcd_contrast } },
	{ "LCD Backlight Time", 			MENU_ITEM_DEFAULT, { menu_set_lcd_backlight } }, 
	{ "Reset temperature", 				MENU_ITEM_DEFAULT, { menu_reset_temperature } },
	{ "Temperature Display Time", 		MENU_ITEM_DEFAULT, { menu_set_temp_disp_time } },
	{ "Time Display Time", 				MENU_ITEM_DEFAULT, { menu_set_time_disp_time } },
	{ "Nameday Display Time", 			MENU_ITEM_DEFAULT, { menu_set_nameday_disp_time } },
	{ "Words Of Wisdom Display Time",	MENU_ITEM_DEFAULT, { menu_set_wow_disp_time } },
	{ "Save Settings to EEPROM", 		MENU_ITEM_DEFAULT, { menu_save_settings } }
};


struct menu g_main_menu = {
	.items = items,
	.parent = NULL,
	.cnt = MENU_ITEMS_SIZE(items),
	._cursor = 0,
	._is = NULL,
	._pd = NULL
};

/* ================================================================================ */


static void menu_set_time(void *pd, uint8_t a_event) {
	// TODO implement me
}


static void menu_set_date(void *pd, uint8_t a_event) {
	/* static uint8_t position = 0; */
	/* uint8_t data[2] = {DS1307_DOM_ADDR}; */

	/* switch(position) { */
	/* 	case 0: */
	/* 		break; */

	/* 	case 1: */
	/* 		break; */

	/* 	case 2: */
	/* 		break; */

	/* 	default: */
	/* 		break; */
	/* } // switch */

	/* snprintf((char *)g_sys_ctx.display[1],  */
	/* 		LCD_CHARACTERS_PER_LINE + 1, " %4d-%02x-%02x     ", */
	/* 		BCD2BIN(g_sys_ctx.tm.year) + 2000, */
	/* 		g_sys_ctx.tm.month, */
	/* 		g_sys_ctx.tm.dom); */
}


static void menu_reset_temperature(void *pd, uint8_t a_event) {
	// TODO implement me
}


static void menu_set_time_mode(void *pd, uint8_t a_event) {
	uint8_t data[2] = {DS1307_HOURS_ADDR};
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	const char *mode[] = {
		"24 mode",
		"12 AM/PM mode"
	};

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
		case E_EVENT_BUTTON_PLUS:

			twi_mtx(TWI_RTC_ADDR, data, 0x01, 0x00);
			while (fpd->tm->twi->status & E_TWI_BIT_BUSY);
			twi_mrx(TWI_RTC_ADDR, &data[1], sizeof(uint8_t), E_TWI_BIT_SEND_STOP);
			while (fpd->tm->twi->status & E_TWI_BIT_BUSY);

			// toggle
			data[1] ^= 0x40;

			twi_mtx(TWI_RTC_ADDR, data, 2, E_TWI_BIT_SEND_STOP);
			while (fpd->tm->twi->status & E_TWI_BIT_BUSY);
			break;
	} // switch

	snprintf((char *)fpd->lcd->display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			" %-15s",
			mode[(fpd->tm->tm.mode_ampm_hour & 0x40) >> 6]);
}


static void _menu_value_regulator(char *d, uint8_t a_event, volatile uint8_t *a_value) {
	char pg[13] = {0x00};

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (*a_value >= 0x08)
				*a_value -= 0x08;
			else 
				*a_value = 0;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (*a_value < 0xf7)
				*a_value += 0x08;
			else 
				*a_value = 0xff;
			break;
	} // switch

	menu_render_progress_bar(pg, 12, 0, 255, *a_value);
	snprintf(d, 
			LCD_CHARACTERS_PER_LINE + 1, 
			"%03d %s",
			*a_value,
			pg);
}


static void menu_set_lcd_brightness(void *pd, uint8_t a_event) {
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	_menu_value_regulator((char *)fpd->lcd->display[1], 
			a_event, &fpd->ss->lcd_brightness);
	SET_BRIGHTNESS(fpd->ss->lcd_brightness);
}


static void menu_set_lcd_contrast(void *pd, uint8_t a_event) {
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	_menu_value_regulator((char *)fpd->lcd->display[1], a_event, &fpd->ss->lcd_contrast);
	SET_CONTRAST(fpd->ss->lcd_contrast);
}


static void _menu_time_regulator(char *d, int8_t *idx, 
		volatile uint8_t *a_value, 
		uint8_t a_event) {
	uint8_t times[] = { 5, 10, 15, 20, 30, 60, 90, 120 };

	// perform initialization
	if (-1 == *idx) {
		*idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < sizeof(times); i++) {
			if (*a_value == times[i]) {
				*idx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (*idx) (*idx)--;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (*idx<(sizeof(times) - 1)) (*idx)++;
			break;
	} // switch

	*a_value = times[*idx];
	snprintf(d, 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %2d secs       ",
			times[*idx]);
}



static void menu_set_lcd_backlight(void *pd, uint8_t a_event) {
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
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	
	// perform initialization
	if (-1 == idx) {
		idx = 0;
		// search for configuration
		for (uint8_t i = 0; i < mc; i++) {
			if (fpd->ss->lcd_bt_time == times[i]) {
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

	fpd->ss->lcd_bt_time = times[idx];
	snprintf((char *)fpd->lcd->display[1], 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %-14s",
			backlight[idx]);
}


static void menu_set_temp_disp_time(void *pd, uint8_t a_event) {
	static int8_t idx = -1;
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	_menu_time_regulator((char *)fpd->lcd->display[1], &idx, &fpd->ss->temp_time, a_event);
}


static void menu_set_time_disp_time(void *pd, uint8_t a_event) {
	static int8_t idx = -1;
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	_menu_time_regulator((char *)fpd->lcd->display[1], &idx, &fpd->ss->time_time, a_event);
}


static void menu_set_nameday_disp_time(void *pd, uint8_t a_event) {
	static int8_t idx = -1;
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	_menu_time_regulator((char *)fpd->lcd->display[1], &idx, &fpd->ss->nm_time, a_event);
}


static void menu_set_wow_disp_time(void *pd, uint8_t a_event) {
	static int8_t idx = -1;
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	_menu_time_regulator((char *)fpd->lcd->display[1], &idx, &fpd->ss->pv_time, a_event);
}


static void menu_save_settings(void *pd, uint8_t a_event) {
	static uint8_t cnt = 30;
	struct fsm_pd *fpd = (struct fsm_pd *)pd;

	if (E_EVENT_BUTTON_OK == a_event) {
		// write the settings structure back to EEPROM
		sys_settings_set(fpd->ss);
		return;
	}

	sprintf((char *)fpd->lcd->display[1], " SETTINGS SAVED ");
	_delay_ms(100);
	cnt--;

	if (!cnt--) {
		// press OK for the user :)
		cnt = 30;
		fsm_event_push(fpd->eq, E_EVENT_BUTTON_OK);
	}
}


/* ================================================================================ */
