#include "menu.h"
#include <stdio.h>


void menu_render(struct menu *a_menu, uint8_t a_elem, char *a_buffer, uint8_t a_linelen) {

	sprintf(a_buffer, "%c%.*s",
			!a_elem ? '>' : ' ',
			a_linelen - 1,
			a_menu->items[a_menu->_curr].name);
}
