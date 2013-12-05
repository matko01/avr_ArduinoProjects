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

#include <stdint.h>


/**
 * @brief maximum length of the menu item's name
 */
#define MENU_ITEM_MAX_LENGTH 32


/**
 * @brief callback type definition
 */
typedef void (*menu_callback_t)(void*);


#define MENU_ITEM_INACTIVE 0x02
#define MENU_ITEM_SUBMENU 0x01

// default item configuration
#define MENU_ITEM_DEFAULT 0x00

#define MENU_ITEMS_SIZE(__items) (sizeof(__items)/sizeof(struct menu_item))

#define MENU_ARROW_UP 0x14
#define MENU_ARROW_DOWN 0x15

/**
 * @brief menu item definition
 */
struct menu_item {
	char name[MENU_ITEM_MAX_LENGTH];

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

	// internal
	uint8_t _curr;
};


void menu_render(struct menu *a_menu, uint8_t a_elem, char *a_buffer, uint8_t a_linelen);


#endif /* MENU_H_VHOFYXWZ */

