#ifndef MENU_H_VHOFYXWZ
#define MENU_H_VHOFYXWZ

/* Copyright (C) 
 * 2013 - Tomasz Wisniewski
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include "lcd.h"
#include <stdint.h>


/**
 * @brief callback type definition
 */
typedef void (*menu_callback_t)(void*,uint8_t);


/// config flag definitions
#define MENU_ITEM_INACTIVE 0x02
#define MENU_ITEM_SUBMENU 0x01

// if this bit is set the suboption is responsible
// for terminating it's own execution
#define MENU_ITEM_OWNER	0x04

/// default item configuration
#define MENU_ITEM_DEFAULT 0x00


/// used to determine how many items are declared
#define MENU_ITEMS_SIZE(__items) (sizeof(__items)/sizeof(struct menu_item))


/**
 * @brief menu item definition
 */
struct menu_item {
	const char *name;

	// 1 - inactive/active
	// 0 - cb/submenu
	uint8_t config;

	union {
		menu_callback_t cb;
		struct menu *submenu;
	} ptr;
};


/**
 * @brief menu definition
 */
struct menu {
	struct menu_item *items;
	struct menu *parent;
	uint8_t cnt;	

	// cursor position
	uint8_t _cursor;

	// currently selected menu item
	struct menu_item *_is;

	// private data
	void *_pd;
};


void menu_render(struct lcd_ctx *a_lcd, struct menu *a_menu);
void menu_process_input(struct menu *a_menu, uint8_t a_input);
void menu_set_private_data(struct menu *a_menu, void *a_data);
void menu_render_progress_bar(char *a_buffer, uint8_t a_len, uint8_t a_min, uint8_t a_max, uint8_t a_cur);


#endif /* MENU_H_VHOFYXWZ */

