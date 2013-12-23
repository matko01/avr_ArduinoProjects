#include "menu.h"
#include "pca.h"
#include "string_util.h"
#include "lcd.h"
#include "int_ctx.h"

#include <string.h>


void menu_render_progress_bar(char *a_buffer, uint8_t a_len, uint8_t a_min, uint8_t a_max, uint8_t a_cur) {
	uint8_t range = a_max - a_min;
	uint8_t pos = a_cur - a_min;
	uint8_t progress = (uint8_t)(((float)pos/range)*a_len);

	if (a_len>3) {
		*a_buffer = '[';
		a_buffer[a_len - 1] = ']';
	}

	for (uint8_t i = 1; i<a_len-1; i++) {
		a_buffer[i] = i<progress ? '#' : ' ';
	}
}


void menu_render(struct lcd_ctx *a_lcd, struct menu *a_menu) {
	static struct scroll_str str = {0x00};
	uint8_t so = (a_menu->_cursor & 0xfe);
	char output[LCD_CHARACTERS_PER_LINE + 1] = {0x00};

	if (NULL != a_menu->_is && 
			!(a_menu->_is->config & MENU_ITEM_INACTIVE)) {

		if (str.s != a_menu->_is->name) str.pos = 0;
		str.s = (char *)a_menu->_is->name;
		str.len = strlen(a_menu->_is->name);

		// get a string slice
		scroll_str_paste(&str, output, sizeof(output) - 1, g_int_ctx._fast_counter);

		// render the function name
		snprintf((char *)a_lcd->display[0], 
				LCD_CHARACTERS_PER_LINE + 1, 
				"%-16s",				
			   	output);

		// ... and the function itself
		if (!(a_menu->_is->config & MENU_ITEM_SUBMENU)) 
			a_menu->_is->ptr.cb(a_menu->_pd, E_EVENT_NONE);

		return;
	}


	// render items
	for (uint8_t i = 0; i<2; i++) {
		const char *istr = a_menu->items[(i + so) % a_menu->cnt].name;
		lcd_clean(a_lcd, i);
		
		if (i + so == a_menu->_cursor) {
			if (str.s != istr) str.pos = 0;
			str.s = (char *)istr;
			str.len = strlen(istr);

			// get a string slice
			scroll_str_paste(&str, output, sizeof(output) - 1, g_int_ctx._fast_counter);
		}

		// render an item
		snprintf((char *)a_lcd->display[i], 
				LCD_CHARACTERS_PER_LINE + 1, 
				"%c%-15s",
				i + so == a_menu->_cursor ? '>' : ' ',
			   	i + so == a_menu->_cursor ? output : istr);
	}

	lcd_blit((struct lcd_ctx *)a_lcd, a_lcd->_vis_pos);
}


void menu_process_input(struct menu *a_menu, uint8_t a_input) {
	if (NULL == a_menu->_is) {
		switch (a_input) {
			case E_EVENT_BUTTON_PLUS:
				a_menu->_cursor = (a_menu->_cursor + 1) % a_menu->cnt;
				break;


			case E_EVENT_BUTTON_MINUS:
				a_menu->_cursor = a_menu->_cursor ?
					(a_menu->_cursor - 1) : (a_menu->cnt - 1);
				break;

			case E_EVENT_BUTTON_OK:
				a_menu->_is = &a_menu->items[a_menu->_cursor];
				break;
		}
	}
	else {
		a_menu->_is->ptr.cb(a_menu->_pd, a_input);
		if (a_input == E_EVENT_BUTTON_OK) a_menu->_is = NULL;
	}			

	_delay_ms(300);
}


void menu_set_private_data(struct menu *a_menu, void *a_data) {
	a_menu->_pd = a_data;
}

