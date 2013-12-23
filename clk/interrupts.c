#include "buttons.h"
#include "int_ctx.h"
#include "fsm.h"
#include "temperature.h"
#include "lcd.h"

#include <avr/interrupt.h>
#include <stdint.h>


// integrated value of the buttons pressed
static volatile uint8_t gs_btn_integration[E_BUTTON_LAST] = {0x00};


/**
 * @brief definition of global variable
 */
volatile struct int_ctx g_int_ctx = {0x00};


// button debounce integration algorithm parameters
#define DEBOUNCE_TIME 0.065
#define DEBOUNCE_SAMPLING_FREQUENCY 61
#define DEBOUNCE_MAXIMUM_PERIOD (DEBOUNCE_TIME * DEBOUNCE_SAMPLING_FREQUENCY)


/**
 * @brief timer 0 overflow interrupt handler
 *
 * @param TIMER0_OVF_vect
 */
ISR(TIMER0_OVF_vect) {
	// buttons
	volatile uint8_t buttons = buttons_get();

	// handle temperature measurements
	tmp_update_tv(&g_int_ctx.tmp->msr);

	// update the back-light
	lcd_update_backlight(g_int_ctx.lcd);

	// update the lcd transition counters
	lcd_update_transition(g_int_ctx.lcd, g_int_ctx.eq);


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
			buttons &= ~_BV(i);
		}
		else if (gs_btn_integration[i]) {
			buttons |= _BV(i);
		}
	} // for

	buttons_set(buttons);

	// fast counter increment
	// will overflow every ~16.5 minutes
	g_int_ctx._fast_counter++;

}



/**
 * @brief external interrupt - 1Hz RTC output line triggers this interrupt
 *
 * PORTD pin 2
 *
 * @param INT0_vect
 */
ISR(INT0_vect) {

	//  every 1 Hz event generation
	fsm_event_push(g_int_ctx.eq, E_EVENT_1HZ);

	// toggle the LED
	GPIO_TOGGLE(g_int_ctx.led);

	// trigger temperature measurement
	tmp_trigger_measurement(g_int_ctx.tmp);

	// generate the time out event
	if (g_int_ctx._event_timer) {
		if (!--g_int_ctx._event_timer) {
			fsm_event_push(g_int_ctx.eq, E_EVENT_TO);
		}
	}

	// update the back light timer
	lcd_update_backlight_timer(g_int_ctx.lcd);
}
