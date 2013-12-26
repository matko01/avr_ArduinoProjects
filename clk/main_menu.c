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
	{ "Set Time", 						MENU_ITEM_OWNER, { menu_set_time } }, 
	{ "Set Date", 						MENU_ITEM_OWNER, { menu_set_date } }, 
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
	struct fsm_pd *fpd = (struct fsm_pd *)pd;

	const char *ms[] = {
		"AM", "PM"
	};

	// strings
	char hour[3] = {0x00};
	char min[3] = {0x00};
	char sec[3] = {0x00};
	char *ptr = NULL;

	static uint8_t mode = 0;
	static uint8_t half = 0;

	static uint8_t position = 0;
	static ds1307_time_t tm = {0x00};
	static struct blink_str bstr = {0x00};
	static uint8_t force = 0;
	static char tmp[3] = {0x00};
	
	// initialization
	if (bstr.str != tmp) {
		tm = fpd->tm->tm;
		tm.ch_sec = BCD2BIN((tm.ch_sec & 0x3f));
		tm.min = BCD2BIN(tm.min);		
		blink_str_init(&bstr, tmp, ' ');

		mode = tm.mode_ampm_hour & 0x40; // high = 12 hour mode
		half = (tm.mode_ampm_hour & 0x20) >> 5;
		tm.mode_ampm_hour = tm.mode_ampm_hour & (mode ? 0x1f : 0x3f);
		tm.mode_ampm_hour = BCD2BIN(tm.mode_ampm_hour);
	}

	// event integration
	if (force) force--;

	sprintf(hour, "%2d", tm.mode_ampm_hour);
	sprintf(min, "%02d", tm.min);
	sprintf(sec, "%02d", tm.ch_sec);

	switch(position) {
		case 0: 
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					force = 16;
					if (mode) {
						if (tm.mode_ampm_hour <= 1) {
							tm.mode_ampm_hour = 12;
							half = (half + 1) & 0x01;
						}
						else {
							tm.mode_ampm_hour--;
						}
					}
					else {
						tm.mode_ampm_hour = tm.mode_ampm_hour ?
							(tm.mode_ampm_hour - 1) : 23;
					}
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					force = 16;
					if (mode) {
						if (tm.mode_ampm_hour == 12) {
							tm.mode_ampm_hour = 1;
							half = (half + 1) & 0x01;
						}
						else {
							tm.mode_ampm_hour = tm.mode_ampm_hour == 23 ?
								0 : (tm.mode_ampm_hour + 1);
						}
					}
					else {
						tm.mode_ampm_hour = (tm.mode_ampm_hour + 1) % 24;
					}
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					position = 1;
				}

				sprintf(tmp, "%2d", tm.mode_ampm_hour);
				ptr = hour;
			}
			break;

		case 1:
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					if (tm.min)
						tm.min--;
					else 
						tm.min = 59;
					force = 16;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.min = (tm.min + 1) % 60;
					force = 16;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					position = 2;
				}

				sprintf(tmp, "%02d", tm.min);
				ptr = min;
			}
			break;

		case 2:
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					if (tm.ch_sec)
						tm.ch_sec--;
					else 
						tm.ch_sec = 59;
					force = 16;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.ch_sec = (tm.ch_sec + 1) % 60;
					force = 16;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					++position;
				}

				sprintf(tmp, "%02d", tm.ch_sec);
				ptr = sec;
			}
			break;

		default:
			position = 0;
			bstr.str = NULL;

			// transmit data to the RTC
			{
				uint8_t data[4] = {0x00};
				data[1] = BIN2BCD(tm.ch_sec);
				data[2] = BIN2BCD(tm.min);

				if (mode)
					data[3] = (BIN2BCD(tm.mode_ampm_hour) | 0x40 | ((half & 0x01) << 5));
				else
					data[3] = (BIN2BCD(tm.mode_ampm_hour) & 0x3f);

				twi_mtx(TWI_RTC_ADDR, data, sizeof(data), E_TWI_BIT_SEND_STOP);
				while (fpd->tm->twi->status & E_TWI_BIT_BUSY);
			}

			// this is cheating - but I'm trying to save flash space
			g_main_menu._is = NULL; 
			break;
	} // switch

	bstr.len = strlen(tmp);
	blink_str_paste(&bstr, ptr, 2, force, g_int_ctx._fast_counter);
	snprintf((char *)fpd->lcd->display[1], 
			LCD_CHARACTERS_PER_LINE + 1, " %2s-%2s-%2s %2s    ",
			hour,
			min,
			sec,
			mode ? ms[half] : " ");
}


static void menu_set_date(void *pd, uint8_t a_event) {
	struct fsm_pd *fpd = (struct fsm_pd *)pd;

	// strings
	char year[5] = {0x00};
	char mon[3] = {0x00};
	char dom[3] = {0x00};

	static uint8_t position = 0;
	static ds1307_time_t tm = {0x00};
	static struct blink_str bstr = {0x00};
	static uint8_t force = 0;
	static char tmp[5] = {0x00};

	uint8_t size = 0;
	char *ptr = NULL;
	
	// initialization
	if (bstr.str != tmp) {
		tm = fpd->tm->tm;
		tm.year = BCD2BIN(tm.year);
		tm.month = BCD2BIN(tm.month);
		tm.dom = BCD2BIN(tm.dom);

		blink_str_init(&bstr, tmp, ' ');
	}

	// event integration
	if (force) force--;
	sprintf(year, "%04d", tm.year + 2000);
	sprintf(mon, "%02d", tm.month);
	sprintf(dom, "%02d", tm.dom);

	switch(position) {
		case 0: 
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					if (tm.year)
						tm.year--;
					else 
						tm.year = 99;
					force = 16;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.year = (tm.year + 1) % 100;
					force = 16;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					position = 1;
				}

				sprintf(tmp, "%4d", tm.year + 2000);
				ptr = year;
				size = sizeof(year) - 1;

			}
			break;

		case 1:
			{
				if (E_EVENT_BUTTON_MINUS == a_event) {
					tm.month--;
					if (0 == tm.month) tm.month = 12;
					force = 16;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.month++;
					if (12 < tm.month) tm.month = 1;
					force = 16;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					position = 2;
				}

				sprintf(tmp, "%02d", tm.month);
				ptr = mon;
				size = sizeof(mon) - 1;
			}
			break;

		case 2:
			{
				uint8_t d = get_month_days(tm.month, tm.year + 2000);
				if (E_EVENT_BUTTON_MINUS == a_event) {
					tm.dom--;
					force = 16;
				}
				else if (E_EVENT_BUTTON_PLUS == a_event) {
					tm.dom++;
					force = 16;
				}
				else if (E_EVENT_BUTTON_OK == a_event) {
					++position;
				}
 
				tm.dom = tm.dom > d ? 1 : (tm.dom == 0 ? d : tm.dom);

				sprintf(tmp, "%02d", tm.dom);
				ptr = dom;
				size = sizeof(dom) - 1;
			}
			break;

		default:
			position = 0;
			bstr.str = NULL;

			// transmit data to the RTC
			{
				tm.dow = get_day_of_week(tm.year, tm.month, tm.dom) + 1;
				tm.year = BIN2BCD(tm.year);
				tm.dom = BIN2BCD(tm.dom);
				tm.month = BIN2BCD(tm.month);
				tm.mode_ampm_hour = DS1307_DOW_ADDR;

				twi_mtx(TWI_RTC_ADDR, &tm.mode_ampm_hour, 5, E_TWI_BIT_SEND_STOP);
				while (fpd->tm->twi->status & E_TWI_BIT_BUSY);
			}

			// this is cheating - but I'm trying to save flash space
			g_main_menu._is = NULL; 
			break;
	} // switch

	bstr.len = strlen(tmp);
	blink_str_paste(&bstr, ptr, size, force, g_int_ctx._fast_counter);

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
			if ((data[1] & 0x40)) {
				uint8_t hour = BCD2BIN((data[1] & 0x3f));
				// correct for 12h mode				
				if (hour >= 12) {
					hour -= 12;
					hour = hour ? hour : 12;
					data[1] |= 0x20; // PM
				}
				else {
					data[1] &= ~0x20; // AM
				}
				data[1] &= 0xe0;
				data[1] |= (BIN2BCD(hour) & 0x1f);
			}
			else {
				// correct for 24h mode
				uint8_t hour = BCD2BIN(data[1] & 0x1f);
				if (data[1] & 0x20) {
					hour = hour == 12 ? 12 : (hour + 12);
				}
				else {
					hour = hour == 12 ? 0 : hour;
				}

				data[1] &= 0xc0;
				data[1] |= (BIN2BCD(hour) & 0x3f);
			}

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
