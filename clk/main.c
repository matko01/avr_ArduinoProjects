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


void main(void) {

	uint8_t size = sizeof(struct sys_ctx);

	// initialize the global context
	common_zero_mem(&g_sys_ctx, size);
	SET_CONTRAST(0x00);

	// setup state machine
	fsm_setup_cb((f_state_cb *)g_sys_ctx.state_cb);
	
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

	// restore saved contrast value
	SET_CONTRAST(g_sys_ctx.settings.lcd_contrast);

	// initialize the FSM
	// initial state = 0 (TIME)	
	g_sys_ctx._event_timer = g_sys_ctx.settings.time_time;

	// now it's safe to globally enable ints
	// HW should be up & running
	sei();

	// execution loop
	for (;;) {
		// pop an event
		uint8_t event = fsm_event_pop(&g_sys_ctx.eq);

		// perform temperature measurement
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

		g_sys_ctx.fsm.cs = 
			g_sys_ctx.state_cb[g_sys_ctx.fsm.cs](&g_sys_ctx, event);
	
	} // for

} // main

