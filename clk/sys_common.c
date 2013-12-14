#include "sys_common.h"
#include "sys_conf.h"

#include <avr/eeprom.h>
#include <avr/power.h>
#include <util/delay.h>


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
	EICRA = 0x02;
	EIMSK = 0x01;
	PORTD &= ~_BV(PORTD2);
	DDRD &= ~_BV(PORTD2);

	// enable output on PORTB3	
	OCR2A = 0x00;
	DDRB |= _BV(PORTB3);
	DDRD |= _BV(PORTD3);
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

		a_ss->temp_time = 14;
		a_ss->time_time = 20;

		// initialize
		eeprom_write_block(a_ss, (void *)0x00, sizeof(struct sys_settings));
	}
}


void displayTime(volatile struct sys_ctx *a_ctx) {

	const char *weekdays[] = {
		"Mon",
		"Tue",
		"Wed",
		"Thu",
		"Fri",
		"Sat",
		"Sun"
	};
	uint8_t x = a_ctx->tm.dow ? a_ctx->tm.dow - 1 : 0x00;

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
			LCD_CHARACTERS_PER_LINE + 1, "%4d-%02x-%02x [%s]",
			BCD2BIN(a_ctx->tm.year) + 2000,
			a_ctx->tm.month,
			a_ctx->tm.dom,
			weekdays[x]); 

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
