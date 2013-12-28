#include "sys_display.h"
#include "string_util.h"
#include "int_ctx.h"
#include "temperature.h"
#include "sys_util.h"
#include "namedays.h"
#include "proverb.h"

#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>


/* ================================================================================ */

static void _pgmspace_display(struct lcd_ctx *a_lcd, const char *a_title, char *data);

/* ================================================================================ */

	
void display_time(volatile struct lcd_ctx *a_lcd, volatile struct time_ctx *tm) {
	const char *weekdays[] = {
		"Saturday",
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
	};
	uint8_t x = tm->tm.dow % 7;
	char output[5] = {0x00};
	uint8_t len = sizeof(output);
	static struct scroll_str str = {0x00};

	common_zero_mem(output, len);

	// zero the position if day has changed
	if (str.s != weekdays[x]) str.pos = 0;	
	str.s = (char *)weekdays[x];
	str.len = strlen(weekdays[x]);

	// get a string slice
	scroll_str_paste(&str, output, sizeof(output) - 1, g_int_ctx._fast_counter);

	if (tm->tm.mode_ampm_hour & 0x40) {
		// 12 hour mode
		sprintf((char *)a_lcd->display[0], 
				"%2x:%02x:%02x %-7s",
				tm->tm.mode_ampm_hour & 0x1f,
				tm->tm.min,
				tm->tm.ch_sec & 0x7f,
				tm->tm.mode_ampm_hour & 0x20 ? "PM" : "AM");
	}
	else {
		sprintf((char *)a_lcd->display[0], 
				"%2x:%02x:%02x %7s",
				tm->tm.mode_ampm_hour & 0x3f,
				tm->tm.min,
				tm->tm.ch_sec & 0x7f,
				" ");
	}

	snprintf((char *)a_lcd->display[1], 
			LCD_CHARACTERS_PER_LINE + 1,
			"%4d-%02x-%02x, %4s",
			BCD2BIN(tm->tm.year) + 2000,
			tm->tm.month,
			tm->tm.dom,
			output); 

	lcd_blit(a_lcd, 0);
}


void display_temp(volatile struct lcd_ctx *a_lcd, volatile struct temp_msr_ctx *temp) {
	snprintf((char *)a_lcd->display[0], LCD_CHARACTERS_PER_LINE + 1, "Current: %2.02f\xdf%c",
			((float)temp->temp)/16, 'C'); 

	snprintf((char *)a_lcd->display[1], LCD_CHARACTERS_PER_LINE + 1, "-/+ %2.02f %2.02f ",
			((float)temp->temp_min)/16,
			((float)temp->temp_max)/16); 

	lcd_blit(a_lcd, 1);
}


void display_nameday(struct lcd_ctx *a_lcd, ds1307_time_t *tm) {
	char ss_mem[40] = "";
	uint16_t yd = (get_year_day(tm) - 1) % 366;

	// make an index correction for non leap years
	// since the FLASH table includes the leap year name-day as well
	if (!is_leap_year(BCD2BIN(tm->year)) && yd >= 59) { // first of march
		// increment the index to ommit the leap year extra day
		yd++;		
	}

	// copy data from FLASH
	strcpy_PF(ss_mem, pgm_read_word(&g_namedays[yd]));
	_pgmspace_display(a_lcd, "Nameday of: ", ss_mem);
}


void display_proverb(struct lcd_ctx *a_lcd, ds1307_time_t *tm) {
	char ss_mem[128] = "";
	uint16_t yd = get_year_day(tm) - 1;

	// wrap around since wo don't have enough for every year day
	yd = yd % PROVERBS_AVAILABLE;

	// copy data from FLASH
	strcpy_PF(ss_mem, pgm_read_word(&g_proverbs[yd]));
	_pgmspace_display(a_lcd, "Words Of Wisdom:", ss_mem);
}

/* ================================================================================ */

static void _pgmspace_display(struct lcd_ctx *a_lcd, const char *a_title, char *data) {
	char output[LCD_CHARACTERS_PER_LINE + 1] = {0x00};
	static struct scroll_str str = {0x00};

	if (data!=str.s) {
		// update the string
		scroll_str_init(&str, data, 0);
	}

	// length of the original string
	str.len = strlen(data);

	// get a string slice
	scroll_str_paste(&str, output, sizeof(output) - 1, g_int_ctx._fast_counter);

	snprintf((char *)a_lcd->display[0], 
			LCD_CHARACTERS_PER_LINE + 1,
			"%-16s", a_title);
	snprintf((char *)a_lcd->display[1], LCD_CHARACTERS_PER_LINE + 1, 
			"%-16s", output); 

	lcd_blit(a_lcd, 1);
}

/* ================================================================================ */

