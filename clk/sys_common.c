#include "sys_common.h"
#include "sys_conf.h"
#include "namedays.h"
#include "proverb.h"
#include "scroll_string.h"
#include "menu.h"

#include <avr/eeprom.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>


void timers_setup() {
	power_timer0_enable();
	power_timer2_enable();

	// fast PWM mode on OC2A (PB3) - OC2B (PD3)
	// in non interting mode
	// prescaler = 256
	TCCR2A = 0xa3;
	TCCR2B = 0x01;

	// timer 0 in normal mode (61 Hz)
	// will be used as a time source
	TCCR0A = 0x00;
	TCCR0B = 0x05;
	TIMSK0 = 0x01;

	// enable external int0 (pd2) on falling edge
	// RTC 1 HZ output
	EICRA = 0x02;
	EIMSK = 0x01;
	PORTD &= ~_BV(PORTD2);
	DDRD &= ~_BV(PORTD2);

	// enable output on PORTB3	
	OCR2A = 0x00;
	DDRB |= _BV(PORTB3);
	DDRD |= _BV(PORTD3);

	DDRC &= 0xf0; // configure first PORTC ports as inputs
	PORTC |= 0x0f; // enable pull-ups 
}


void led_setup(volatile struct sys_ctx *a_ctx) {
	a_ctx->led.port = &RTC_LED_PORT;
	a_ctx->led.pin = RTC_LED_PIN;

	GPIO_CONFIGURE_AS_OUTPUT(&a_ctx->led);
	GPIO_SET_HIGH(&a_ctx->led);
}


void rtc_setup(volatile struct twi_ctx *a_ctx) {
	uint8_t data[9] = {DS1307_CONTROL_ADDR};
	uint8_t len = 0x00;

	twi_mtx(TWI_RTC_ADDR, data, 0x01, 0x00);
	while (a_ctx->status & E_TWI_BIT_BUSY);
	
	twi_mrx(TWI_RTC_ADDR, &data[1], sizeof(uint8_t), E_TWI_BIT_SEND_STOP);
	while (a_ctx->status & E_TWI_BIT_BUSY);

	// out = 0
	// sqw = 1
	// rs = 0
	if (0x10!=data[1]) {
		// the clock is not properly initialized
		len = sizeof(data);
		common_zero_mem(data,len);

		// TODO temporary changed, should be 0x80 - HALTED
		data[1] = 0x10; // clock halted

		data[1 + DS1307_DOW_ADDR] = 1;
		data[1 + DS1307_DOM_ADDR] = 1;
		data[1 + DS1307_MONTHS_ADDR] = 1;

		data[DS1307_CONTROL_ADDR + 1] = 0x10;
		twi_mtx(TWI_RTC_ADDR, data, sizeof(data), E_TWI_BIT_SEND_STOP);
		while (a_ctx->status & E_TWI_BIT_BUSY);
	}
}


void bus_ow_setup(struct soft_ow *a_sow_ctx) {

	// soft one wire bus setup
	a_sow_ctx->conf = 
		SOFT_OW_CONF_MAKE(OW_POWER_PARASITE, OW_SINGLEDROP);

	a_sow_ctx->bus.port = &OW_PORT;
	a_sow_ctx->bus.pin = OW_PIN;

	// initialize one-wire
	sow_init(a_sow_ctx);
}


void bus_twi_setup(struct twi_ctx **a_ctx) {
	// I2C CTX
	*a_ctx = (struct twi_ctx *)twi_init(E_TWI_SCL_100K);
}


void sys_settings_get(struct sys_settings *a_ss) {
	eeprom_read_block(a_ss, (void *)0x00, sizeof(struct sys_settings));

	// initialize if eeprom doesn't contain
	// any settings yet
	if (SETTINGS_MAGIC_ID != a_ss->magic) {
		a_ss->magic = SETTINGS_MAGIC_ID;
		a_ss->lcd_brightness = 0xff;
		a_ss->lcd_contrast = 0xff;
		a_ss->lcd_bt_time = 0;

		/* a_ss->temp_time = 14; */
		/* a_ss->time_time = 20; */
		/* a_ss->nm_time = 10; */

		a_ss->temp_time = 10;
		a_ss->time_time = 10;
		a_ss->nm_time = 10;
		a_ss->pv_time = 30;

		// initialize
		eeprom_write_block(a_ss, (void *)0x00, sizeof(struct sys_settings));
	}
}


void displayTime(volatile struct sys_ctx *a_ctx) {

	const char *weekdays[] = {
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		"Sunday"
	};
	uint8_t x = a_ctx->tm.dow ? a_ctx->tm.dow - 1 : 0x00;
	char output[5] = {0x00};
	uint8_t len = sizeof(output);
	static struct scroll_str str = {0x00};

	common_zero_mem(output, len);

	// zero the position if day has changed
	if (str.s != weekdays[x]) str.pos = 0;	
	str.s = (char *)weekdays[x];
	str.len = strlen(weekdays[x]);

	// get a string slice
	scroll_str_paste(&str, output, sizeof(output) - 1, a_ctx->_fast_counter);

	if (a_ctx->tm.mode_ampm_hour & 0x40) {
		// 12 hour mode
		snprintf((char *)a_ctx->display[0], 
				LCD_CHARACTERS_PER_LINE + 1, "%2x:%02x:%02x %s",
				a_ctx->tm.mode_ampm_hour & 0x1f,
				a_ctx->tm.min,
				a_ctx->tm.ch_sec & 0x7f,
				a_ctx->tm.mode_ampm_hour & 0x20 ? "PM" : "AM");
	}
	else {
		snprintf((char *)a_ctx->display[0], 
				LCD_CHARACTERS_PER_LINE + 1, "%2x:%02x:%02x",
				a_ctx->tm.mode_ampm_hour & 0x3f,
				a_ctx->tm.min,
				a_ctx->tm.ch_sec & 0x7f);
	}

	snprintf((char *)a_ctx->display[1], 
			LCD_CHARACTERS_PER_LINE + 1, "%4d-%02x-%02x, %4s",
			BCD2BIN(a_ctx->tm.year) + 2000,
			a_ctx->tm.month,
			a_ctx->tm.dom,
			output); 

	// TODO maybe think about some other method 
	// not to block interrupts so often
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE00_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[0]);
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE10_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[1]);
	}
}


void displayTemp(volatile struct sys_ctx *a_ctx) {
	snprintf((char *)a_ctx->display[0], LCD_CHARACTERS_PER_LINE + 1, "Current: %2.02f\xdf%c",
			((float)a_ctx->temp_ctx.temp)/16, 'C'); 

	snprintf((char *)a_ctx->display[1], LCD_CHARACTERS_PER_LINE + 1, "-/+ %2.02f %2.02f ",
			((float)a_ctx->temp_ctx.temp_min)/16,
			((float)a_ctx->temp_ctx.temp_max)/16); 

	// TODO maybe think about some other method 
	// not to block interrupts so often
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE01_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[0]);
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE11_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[1]);
	}
}


uint8_t is_leap_year(uint8_t a_year) {
	uint16_t y = a_year + 2000;
	return ((y%4 == 0 && y%100 != 0) || y%400 == 0);
}


uint16_t get_year_day(volatile struct sys_ctx *a_ctx) {
	uint16_t day = 0;
	uint8_t mon[] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};

	if (is_leap_year(a_ctx->tm.year)) {
		mon[1] = 29;
	}

	for (int8_t i = (a_ctx->tm.month - 1); i>0; i--) {
		day += mon[i - 1];
	}

	day += a_ctx->tm.dom;
	return day;
}


void displayNameday(volatile struct sys_ctx *a_ctx) {
	char ss_mem[40] = "";
	char output[LCD_CHARACTERS_PER_LINE + 1] = {0x00};
	static struct scroll_str str = {0x00};
	uint16_t yd = get_year_day(a_ctx) - 1;

	if (!str.s) {
		scroll_str_init(&str, ss_mem, 0);
	}

	// make an index correction for non leap years
	// since the FLASH table includes the leap year name-day as well
	if (!is_leap_year(a_ctx->tm.year) && yd >= 59) { // first of march
		// increment the index to ommit the leap year extra day
		yd++;		
	}

	// copy data from FLASH
	strcpy_PF(ss_mem, pgm_read_word(&g_namedays[yd]));

	// length of the original string
	str.len = strlen(ss_mem);

	// get a string slice
	scroll_str_paste(&str, output, sizeof(output) - 1, a_ctx->_fast_counter);

	snprintf((char *)a_ctx->display[0], LCD_CHARACTERS_PER_LINE + 1, "Nameday:        ");
	snprintf((char *)a_ctx->display[1], LCD_CHARACTERS_PER_LINE + 1, "%s", output); 

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE01_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[0]);
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE11_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[1]);
	}
}


void displayProverb(volatile struct sys_ctx *a_ctx) {
	char ss_mem[128] = "";
	char output[LCD_CHARACTERS_PER_LINE + 1] = {0x00};
	static struct scroll_str str = {0x00};
	uint16_t yd = get_year_day(a_ctx) - 1;

	if (!str.s) {
		scroll_str_init(&str, ss_mem, 0);
	}

	// wrap around since wo don't have enough for every year day
	yd = yd % PROVERBS_AVAILABLE;

	// copy data from FLASH
	strcpy_PF(ss_mem, pgm_read_word(&g_proverbs[yd]));

	// length of the original string
	str.len = strlen(ss_mem);

	// get a string slice
	scroll_str_paste(&str, output, sizeof(output) - 1, a_ctx->_fast_counter);

	snprintf((char *)a_ctx->display[0], LCD_CHARACTERS_PER_LINE + 1, "Words of Wisdom:");
	snprintf((char *)a_ctx->display[1], LCD_CHARACTERS_PER_LINE + 1, "%s", output); 

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE01_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[0]);
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, LCD_LINE11_ADDR);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[1]);
	}
}


void displayMenu(volatile struct sys_ctx *a_ctx) {
	uint8_t p[2] = {0x00};

	// determine the render position
	p[0] = a_ctx->_vis_pos ? LCD_LINE01_ADDR : LCD_LINE00_ADDR;
	p[1] = a_ctx->_vis_pos ? LCD_LINE11_ADDR : LCD_LINE10_ADDR;

	menu_render(a_ctx->menu);
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, p[0]);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[0]);
		hd44780_goto((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx, p[1]);
		hd44780_puts((struct dev_hd44780_ctx *)&a_ctx->lcd_ctx,(char *)a_ctx->display[1]);
	}
}
