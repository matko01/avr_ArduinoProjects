#include "pca.h"
#include "main.h"
#include "sys_ctx.h"

#include "sys_conf.h"
#include "sys_common.h"
#include "lcd.h"

#include <util/delay.h>
#include <avr/interrupt.h>


/**
 * @brief global system context
 */
volatile struct sys_ctx g_sys_ctx;


void main(void) {

	uint8_t size = sizeof(struct sys_ctx);

	// initialize the global context
	common_zero_mem(&g_sys_ctx, size);
	
	// get system settings from eeprom
	sys_settings_get((struct sys_settings *)&g_sys_ctx.settings);

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


	// execution loop
	for (;;) {
		tmp_update_measurements(&g_sys_ctx.temp_ctx,
				&g_sys_ctx.sow_ctx);

		_delay_ms(50);
	}

} // main



	
	/* for (;;) { */

		/* sprintf(str, "%02x:%02x:%02x",  */
		/* 	tm->mode_ampm_hour & 0x3f, */
		/* 	tm->min, */
		/* 	tm->ch_sec & 0x7f); */

		/* hd44780_goto(&lcd_ctx, 0x00); */
		/* hd44780_puts(&lcd_ctx, str); */

		/* hd44780_goto(&lcd_ctx, 0x40); */
		/* sprintf(str, "%04u/%04u/%04u", temp_min, *temp, temp_max); */
		/* hd44780_puts(&lcd_ctx, str); */

	/* 	hd44780_goto(&lcd_ctx, 0x00); */
	/* 	hd44780_puts(&lcd_ctx, "test"); */
	/* 	hd44780_goto(&lcd_ctx, 0x02); */
	/* } */
