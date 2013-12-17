#include "temperature.h"
#include "main.h"
#include <avr/interrupt.h>
#include "sys_ctx.h"
#include "fsm.h"

// integrated value of the buttons pressed
static volatile uint8_t gs_btn_integration[E_BUTTON_LAST] = {0x00};

#define DEBOUNCE_TIME 0.065
#define DEBOUNCE_SAMPLING_FREQUENCY 61
#define DEBOUNCE_MAXIMUM_PERIOD (DEBOUNCE_TIME * DEBOUNCE_SAMPLING_FREQUENCY)


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
				// scroll left <--
				hd44780_write((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx, 
						HD44780_CMD_CD_SHIFT(1, 0), 0);
		}
		else if (g_sys_ctx._vis_pos < g_sys_ctx._cur_pos) {
			// scroll right -->
			if (!(g_sys_ctx._cur_pos--%8))
				hd44780_write((struct dev_hd44780_ctx *)&g_sys_ctx.lcd_ctx, 
						HD44780_CMD_CD_SHIFT(1, 1), 0);
		}
		
		if (g_sys_ctx._vis_pos == g_sys_ctx._cur_pos) {
			// the transition has ended
			fsm_event_push(&g_sys_ctx.eq, E_EVENT_TRANSITION_END);
		}
	}

	// fast counter increment
	// will overflow every ~16.5 minutes
	g_sys_ctx._fast_counter++;

	// button debounsing procedure
	for (volatile uint8_t i = 0; i<E_BUTTON_LAST; i++) {
		// button pressed - bit is zero
		if (!(PINC & _BV(i))) {
			if (gs_btn_integration[i]) gs_btn_integration[i]--;
		}
		else if (gs_btn_integration[i] < DEBOUNCE_MAXIMUM_PERIOD) {
			// button released
			gs_btn_integration[i]++;
		}

		// the logic here is inverted since low state means button pressed
		if (gs_btn_integration[i] >= DEBOUNCE_MAXIMUM_PERIOD) {
			g_sys_ctx.buttons &= ~_BV(i);
		}
		else if (gs_btn_integration[i]) {
			g_sys_ctx.buttons |= _BV(i);
		}
	} // for
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
