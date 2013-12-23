#ifndef INT_CTX_H_TKN4WTHU
#define INT_CTX_H_TKN4WTHU

#include "pca.h"

#include <stdint.h>


/**
 * @brief this structure aggregates all the data which interrupts must access
 */
struct int_ctx {
	// this is a rolling low-precision counter
	// incremented by the hardware timer every ~16 ms
	// used for text scrolling and animations
	volatile uint16_t _fast_counter;

	// event timer is set by various routines
	// and decrement every second
	// when reaches 0 an EVENT_TO will be queued
	volatile uint8_t _event_timer;

	// pointer to event queue -so, the interrupt is able to generate events
	volatile struct event_queue *eq;

	// pointer to the LED gpio
	volatile gpio_pin *led;

	// temperature measurement context
	volatile struct temp_ctx *tmp;

	// lcd context
	volatile struct lcd_ctx *lcd;
};


/**
 * @brief extern definition of global variable
 */
extern volatile struct int_ctx g_int_ctx;


#endif /* end of include guard: INT_CTX_H_TKN4WTHU */

