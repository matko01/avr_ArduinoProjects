#include "sys_common.h"
#include "sys_conf.h"

#include <avr/eeprom.h>
#include <avr/power.h>


void timers_setup() {
	power_timer0_enable();
	power_timer2_enable();

	// fast PWM mode both OC2A (PB3) and OC2B (PD3)
	// in non interting mode
	// prescaler = 256
	TCCR2A = 0xa3;
	TCCR2B = 0x06;

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
}

#include <util/delay.h>


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

		data[1] = 0x10; // clock halted
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
		a_ss->lcd_brightness = 255;
		a_ss->lcd_bt_time = 0;

		// initialize
		eeprom_write_block(a_ss, (void *)0x00, sizeof(struct sys_settings));
	}
}


