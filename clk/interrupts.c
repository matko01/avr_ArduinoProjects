#include "temperature.h"
#include "main.h"
#include <avr/interrupt.h>
#include "sys_ctx.h"

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

	if (g_sys_ctx.settings.lcd_bt_time) 
		g_sys_ctx.settings.lcd_bt_time--;
}
