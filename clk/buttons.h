#ifndef __BUTTONS_H__
#define __BUTTONS_H__


#include <stdint.h>

/**
 * @brief buttons enumeration
 */
enum e_buttons {
	E_MENU = 0,
	E_MINUS,
	E_PLUS,
	E_OK,

	E_BUTTON_LAST
};

volatile uint8_t buttons_get(void);
void buttons_set(volatile uint8_t);


#endif /* __BUTTONS_H__ */
