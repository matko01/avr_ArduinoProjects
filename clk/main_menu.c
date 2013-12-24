#include "main_menu.h"
#include "menu.h"
#include "fsm_private_data.h"
#include "sys_setup.h"
#include "pca.h"
#include "string_util.h"
#include "int_ctx.h"
#include "sys_util.h"

#include <string.h>

/* ================================================================================ */

static void _menu_value_regulator(char *d, uint8_t a_event, volatile uint8_t *a_value);
static void _menu_time_regulator(char *d, int8_t *idx, volatile uint8_t *a_value, uint8_t a_event);

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
	struct fsm_pd *fpd = (struct fsm_pd *)pd;

	// strings
	char year[5] = {0x00};
	char mon[3] = {0x00};
	char dom[3] = {0x00};
	char tmp[5] = {0x00};

	static uint8_t position = 0;
	static ds1307_time_t tm = {0x00};
	static struct blink_str bstr = {0x00};
	
	// initialization
	if (!tm.dom) {
		tm = fpd->tm->tm;
		tm.year = BCD2BIN(tm.year);
		blink_str_init(&bstr, tmp, ' ');
	}	

	switch(position) {
		case 0: 
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					tm.year--;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.year++;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					position = 1;
				}

				tm.year = tm.year % 100;
				sprintf(tmp, "%4d", tm.year + 2000);
				bstr.len = strlen(tmp);
				blink_str_paste(&bstr, year, sizeof(year)-1, g_int_ctx._fast_counter);
				sprintf(mon, "%02x", tm.month);
				sprintf(dom, "%02x", tm.dom);
			}
			break;

		case 1:
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					tm.month--;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.month++;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					position = 2;
				}

				tm.month = tm.month % 12;
				sprintf(tmp, "%2d", tm.month);
				bstr.len = strlen(tmp);
				blink_str_paste(&bstr, mon, sizeof(mon)-1, g_int_ctx._fast_counter);
				sprintf(year, "%04d", tm.year + 2000);
				sprintf(dom, "%02x", tm.dom);
			}
			break;

		case 2:
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					tm.dom--;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.dom++;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					++position;
				}

				tm.dom = tm.dom % get_month_days(tm.month, tm.year);
				sprintf(tmp, "%2d", tm.dom);
				bstr.len = strlen(tmp);
				blink_str_paste(&bstr, dom, sizeof(dom)-1, g_int_ctx._fast_counter);
				sprintf(year, "%04d", tm.year + 2000);
				sprintf(mon, "%02x", tm.month);
			}
			break;

		default:
			tm.dom = 0;
			position = 0;
			// transmit data to the RTC
			break;
	} // switch

	snprintf((char *)fpd->lcd->display[1], 
			LCD_CHARACTERS_PER_LINE + 1, " %4s-%2s-%2s     ",
			year,
			mon,
			dom);

}


static void menu_reset_temperature(void *pd, uint8_t a_event) {
	// TODO implement me
}


static void menu_set_time_mode(void *pd, uint8_t a_event) {
	uint8_t data[2] = {DS1307_HOURS_ADDR};
	struct fsm_pd *fpd = (struct fsm_pd *)pd;
	const char *mode[] = {
		"24",
		"12 AM/PM"
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
			" %8s mode  ",
			mode[(fpd->tm->tm.mode_ampm_hour & 0x40) >> 6]);
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

static void _menu_time_regulator(char *d, int8_t *idx, 
		volatile uint8_t *a_value, 
		uint8_t a_event) {
	uint8_t times[] = { 5, 10, 15, 20, 30, 60, 90, 120 };

	// avoid working on bare pointer since it produces bigger code
	int8_t aidx = *idx;	

	// perform initialization
	if (-1 == aidx) {
		aidx = 0;
		// search for configuration
		for (uint8_t i = 0; i < sizeof(times); i++) {
			if (*a_value == times[i]) {
				aidx = i;
				break;
			}
		}
	} // if

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			aidx--;
			break;

		case E_EVENT_BUTTON_PLUS:
			aidx++;
			break;
	} // switch

	aidx = aidx % sizeof(times);

	*a_value = times[aidx];
	snprintf(d, 
			LCD_CHARACTERS_PER_LINE + 1, 
			"  %2d secs       ",
			times[aidx]);

	*idx = aidx;
}


static void _menu_value_regulator(char *d, uint8_t a_event, volatile uint8_t *a_value) {
	char pg[13] = {0x00};
	volatile uint8_t av = *a_value;

	switch (a_event) {
		case E_EVENT_BUTTON_MINUS:
			if (av >= 0x08) 
				av -= 0x08;
			else 
				av = 0;
			break;

		case E_EVENT_BUTTON_PLUS:
			if (av < 0xf7)
				av += 0x08;
			else 
				av = 0xff;
			break;
	} // switch

	menu_render_progress_bar(pg, 12, 0, 255, av);
	snprintf(d, 
			LCD_CHARACTERS_PER_LINE + 1, 
			"%03d %s",
			av,
			pg);

	*a_value = av;
}


/* ================================================================================ */
