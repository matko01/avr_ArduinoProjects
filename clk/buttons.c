#include "buttons.h"

volatile static uint8_t gs_buttons = 0x00;


volatile uint8_t buttons_get(void) {
	return gs_buttons;
}


void buttons_set(volatile uint8_t a_buttons) {
	gs_buttons = a_buttons;
}
