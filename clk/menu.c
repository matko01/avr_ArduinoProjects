#include "menu.h"
#include "pca.h"


struct menu_item items[] = {
	{ "Set Time", MENU_ITEM_DEFAULT, {NULL} }, 
	{ "Time Mode", MENU_ITEM_DEFAULT, {NULL} }, 
	{ "Set Date", MENU_ITEM_DEFAULT, {NULL} }, 
	{ "LCD Brightness", MENU_ITEM_DEFAULT, {NULL} }, 
	{ "LCD Backlight", MENU_ITEM_DEFAULT, {NULL} }, 
	{ "LCD Contrast", MENU_ITEM_DEFAULT, {NULL} },
	{ "Temp Disp Time", MENU_ITEM_DEFAULT, {NULL} },
	{ "Time Disp Time", MENU_ITEM_DEFAULT, {NULL} },
	{ "Nameday Disp Time", MENU_ITEM_DEFAULT, {NULL} },
	{ "WoW Disp Time", MENU_ITEM_DEFAULT, {NULL} }
};


struct menu main_menu = {
	.items = items,
	.parent = NULL,
	.cnt = MENU_ITEMS_SIZE(items)
};


void menu_render(struct menu *a_menu, uint8_t a_elem, char *a_buffer, uint8_t a_linelen) {
/* 	sprintf(a_buffer, "%c%.*s", */
/* 			!a_elem ? '>' : ' ', */
/* 			a_linelen - 1, */
/* 			a_menu->items[a_menu->_curr].name); */
}
