#include "menu.h"
#include "pca.h"
#include "fsm.h"

#include <util/delay.h>


static void menu_set_time(void*,uint8_t);
static void menu_set_date(void*,uint8_t);
static void menu_set_time_mode(void*,uint8_t);
static void menu_set_lcd_brightness(void*,uint8_t);
static void menu_set_lcd_contrast(void*,uint8_t);
static void menu_set_lcd_backlight(void*,uint8_t);
static void menu_reset_temperature(void*,uint8_t);
static void menu_set_temp_disp_time(void*,uint8_t);
static void menu_set_time_disp_time(void*,uint8_t);
static void menu_set_nameday_disp_time(void*,uint8_t);
static void menu_set_wow_disp_time(void*,uint8_t);


struct menu_item items[] = {
	{ "Set Time", 			MENU_ITEM_DEFAULT, NULL, { menu_set_time } }, 
	{ "Set Date", 			MENU_ITEM_DEFAULT, NULL, { menu_set_date } }, 
	{ "Time Mode", 			MENU_ITEM_DEFAULT, NULL, { menu_set_time_mode } }, 
	{ "LCD Brightness", 	MENU_ITEM_DEFAULT, NULL, { menu_set_lcd_brightness } }, 
	{ "LCD Contrast", 		MENU_ITEM_DEFAULT, NULL, { menu_set_lcd_contrast } },
	{ "LCD Backlight Time", MENU_ITEM_DEFAULT, NULL, { menu_set_lcd_backlight } }, 
	{ "Reset temperature", 	MENU_ITEM_DEFAULT, NULL, { menu_reset_temperature } },
	{ "Temp Disp Time", 	MENU_ITEM_DEFAULT, NULL, { menu_set_temp_disp_time } },
	{ "Time Disp Time", 	MENU_ITEM_DEFAULT, NULL, { menu_set_time_disp_time } },
	{ "Nameday Disp Time", 	MENU_ITEM_DEFAULT, NULL, { menu_set_nameday_disp_time } },
	{ "WoW Disp Time", 		MENU_ITEM_DEFAULT, NULL, { menu_set_wow_disp_time } }
};


struct menu g_main_menu = {
	.items = items,
	.parent = NULL,
	.cnt = MENU_ITEMS_SIZE(items),
	._cursor = 0,
	._is = NULL
};


void menu_render_progress_bar(char *a_buffer, uint8_t a_len, uint8_t a_min, uint8_t a_max, uint8_t a_cur) {
	uint8_t range = a_max - a_min;
	uint8_t pos = a_cur - a_min;
	uint8_t progress = (uint8_t)(((float)pos/range)*a_len);

	if (a_len>3) {
		*a_buffer = '[';
		a_buffer[a_len - 1] = ']';
	}

	for (uint8_t i = 1; i<a_len-1; i++) {
		a_buffer[i] = i<progress ? '-' : ' ';
	}
}


void menu_render(struct menu *a_menu, char **buffer, uint8_t a_lines, uint8_t a_linelen) {
	uint8_t len = 0;
	uint8_t so = 0;

	if (a_menu->_is) {
		// render the function
		return;
	}
	
	switch (a_lines) {
		case 2:
			// optimized for 2 lines display
			so = a_menu->_cursor & ~_BV(0x01);
			break;

		default:
			// generic formula
			so = (a_menu->_cursor/a_lines) * a_lines;	
			break;
	} // switch


	// render items
	for (uint8_t i = 0; i<a_lines; i++) {

		// clear the buffer
		len = a_linelen;
		common_zero_mem(buffer[i], len);

		// render an item
		snprintf((char *)buffer[i], 
				a_linelen, 
				"$c%s",
				i + so == a_menu->_cursor ? '>' : ' ',
			   	a_menu->items[(i + so) % a_menu->cnt].name);
	}
}


void menu_process_input(struct menu *a_menu, uint8_t a_input) {
	switch (a_input) {
		case E_EVENT_BUTTON_PLUS:
			if (NULL == a_menu->_is) {
				a_menu->_cursor = 
					(a_menu->_cursor + 1) % a_menu->cnt;
			}
			else {
				a_menu->_is->ptr.cb(a_menu->_is->pd, a_input);
			}
			break;

		case E_EVENT_BUTTON_MINUS:
			if (NULL == a_menu->_is) {
				a_menu->_cursor = a_menu->_cursor ?
				   (a_menu->_cursor - 1) : (a_menu->cnt - 1);
			}
			else {
				a_menu->_is->ptr.cb(a_menu->_is->pd, a_input);
			}			
			break;

		case E_EVENT_BUTTON_OK:
			if (NULL == a_menu->_is) {
				a_menu->_is = &a_menu->items[a_menu->_cursor];
			}
			else {
				a_menu->_is = NULL;
			}
			break;
	} // switch
	_delay_ms(50);
}



static void menu_set_time(void *a_data, uint8_t a_event) {
}


static void menu_set_date(void *a_data, uint8_t a_event) {
}


static void menu_set_time_mode(void *a_data, uint8_t a_event) {
}


static void menu_set_lcd_brightness(void *a_data, uint8_t a_event) {
}


static void menu_set_lcd_contrast(void *a_data, uint8_t a_event) {
}


static void menu_set_lcd_backlight(void *a_data, uint8_t a_event) {
}


static void menu_reset_temperature(void *a_data, uint8_t a_event) {
}


static void menu_set_temp_disp_time(void *a_data, uint8_t a_event) {
}


static void menu_set_time_disp_time(void *a_data, uint8_t a_event) {
}


static void menu_set_nameday_disp_time(void *a_data, uint8_t a_event) {
}


static void menu_set_wow_disp_time(void *a_data, uint8_t a_event) {
}



