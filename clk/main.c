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


/**
 * @brief menu definition
 */
extern struct menu g_main_menu;


void main(void) {

	/**
	 * @brief sys_ctx size
	 */
	uint8_t size = sizeof(struct sys_ctx);

	// initialize the global context
	common_zero_mem(&g_sys_ctx, size);
	SET_CONTRAST(0x00);
	SET_BRIGHTNESS(0x00);

	// attach the main menu
	g_sys_ctx.menu = &g_main_menu;

	// setup state machine
	fsm_init((struct fsm_t *)&g_sys_ctx.fsm);
	
	// get system settings from eeprom
	sys_settings_get((struct sys_settings *)&g_sys_ctx.settings);

	// setup the LED
	led_setup((gpio_pin *)&g_sys_ctx.led);

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

	// initialize the FSM
	// initial state = 0 (TIME)	
	g_sys_ctx._event_timer = g_sys_ctx.settings.time_time;

	// now it's safe to globally enable ints
	// HW should be up & running
	sei();

	// restore saved contrast value
	SET_CONTRAST(g_sys_ctx.settings.lcd_contrast);

	serial_init(E_BAUD_9600);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();

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

		// execute the state machine
		g_sys_ctx.fsm.cs = g_sys_ctx.fsm.cs.cb(event);

		// if button pressed
		if (g_sys_ctx.buttons) {
			// refresh the timer
			g_sys_ctx._lcd_backlight_timer = g_sys_ctx.settings.lcd_bt_time;

			// push a key-press event
			fsm_event_push(&g_sys_ctx.eq, 0x10 + g_sys_ctx.buttons);
		}

		// poll every 10 ms (or even longer
		// when considering the processing time)
		_delay_ms(10);
	} // for

} // main

