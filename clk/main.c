#include "pca.h"
#include "main.h"
#include "sys_ctx.h"

#include "sys_conf.h"
#include "sys_common.h"
#include "lcd.h"

#include <util/delay.h>
#include <avr/interrupt.h>

#include "fsm.h"


/**
 * @brief global system context
 */
volatile struct sys_ctx g_sys_ctx;

struct g_fsm_st[] = {
	{  }
};

void main(void) {

	uint8_t size = sizeof(struct sys_ctx);

	// initialize the global context
	common_zero_mem(&g_sys_ctx, size);
	SET_CONTRAST(0x00);
	
	// get system settings from eeprom
	sys_settings_get((struct sys_settings *)&g_sys_ctx.settings);

	// setup the LED
	led_setup(&g_sys_ctx);

	// setup the display	
	lcd_setup((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx);

	// setup buses
	bus_twi_setup((struct twi_ctx **)&g_sys_ctx.twi_ctx);
	bus_ow_setup((struct soft_ow *)&g_sys_ctx.sow_ctx);

	// setup clock
	rtc_setup(g_sys_ctx.twi_ctx);

	// disable interrupts temporary & 
	// continue with HW init
	cli();
	tmp_setup(&g_sys_ctx.temp_ctx, (struct soft_ow *)&g_sys_ctx.sow_ctx);
	timers_setup();

	// now it's safe to globally enable ints
	// HW should be up & running
	sei();

	// restore saved contrast value
	SET_CONTRAST(g_sys_ctx.settings.lcd_contrast);

	// execution loop
	for (;;) {
		tmp_update_measurements(&g_sys_ctx.temp_ctx,
				&g_sys_ctx.sow_ctx);

		// update time measurement
		if (g_sys_ctx._time_trigger) {
			uint8_t ptr = 0x00;

			twi_mtx(TWI_RTC_ADDR, &ptr, 0x01, 0x00);
			while (g_sys_ctx.twi_ctx->status & E_TWI_BIT_BUSY);

			twi_mrx(TWI_RTC_ADDR, (uint8_t *)&g_sys_ctx.tm, 
					sizeof(g_sys_ctx.tm), 
					E_TWI_BIT_SEND_STOP);
			while (g_sys_ctx.twi_ctx->status & E_TWI_BIT_BUSY);

			g_sys_ctx._time_trigger = 0;
		}

		switch(g_sys_ctx.state) {

			case E_STATE_TEMPERATURE:
				displayTemp(&g_sys_ctx);
				break;

			case E_STATE_MENU:
				break;

			default:
			case E_STATE_TIME:
			case E_STATE_LAST:
				displayTime(&g_sys_ctx);
				break;
		} // switch
	} // for

} // main

