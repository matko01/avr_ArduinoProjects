#include "temperature.h"
#include "main.h"
#include <avr/interrupt.h>
#include "sys_ctx.h"
#include "fsm.h"


/**
 * @brief timer 0 overflow interrupt handler
 *
 * @param TIMER0_OVF_vect
 */
ISR(TIMER0_OVF_vect) {
	// handle temperature measurements
	tmp_update_tv(&g_sys_ctx.temp_ctx);

	// fade in / fade out
	if (&g_sys_ctx.lcd_backlight_timer || !g_sys_ctx.settings.lcd_bt_time) {
		if (OCR2A < g_sys_ctx.settings.lcd_brightness) 
			++OCR2A;
		
	}
	else {
		if (OCR2A) OCR2A--;
	}

	if (g_sys_ctx._vis_pos != g_sys_ctx._cur_pos) {

		// screen shift animation
		if (g_sys_ctx._vis_pos > g_sys_ctx._cur_pos) {
			if (!(g_sys_ctx._cur_pos++%8))
				hd44780_write((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx, 
						HD44780_CMD_CD_SHIFT(1, 0), 0);
		}
		else if (g_sys_ctx._vis_pos < g_sys_ctx._cur_pos) {
			if (!(g_sys_ctx._cur_pos--%8))
				hd44780_write((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx, 
						HD44780_CMD_CD_SHIFT(1, 1), 0);
		}
		
		if (g_sys_ctx._vis_pos == g_sys_ctx._cur_pos) {
			// the transition has ended
			fsm_event_push(&g_sys_ctx.eq, E_EVENT_TRANSITION_END);
		}
	}
}


/**
 * @brief external interrupt - 1Hz RTC output line triggers this interrupt
 *
 * PORTD pin 2
 *
 * @param INT0_vect
 */
ISR(INT0_vect) {

	// trigger temperature measurement
	tmp_trigger_measurement(&g_sys_ctx.temp_ctx,
			(struct soft_ow *)&g_sys_ctx.sow_ctx);

	g_sys_ctx._time_trigger = 1;

	// toggle the LED
	GPIO_TOGGLE(&g_sys_ctx.led);

	if (g_sys_ctx.settings.lcd_bt_time) 
		g_sys_ctx.settings.lcd_bt_time--;


	if (g_sys_ctx._event_timer) {
		if (!--g_sys_ctx._event_timer) {
			fsm_event_push(&g_sys_ctx.eq, E_EVENT_TO);
		}
	}
}
