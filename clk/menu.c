#include "menu.h"
#include "pca.h"


static void menu_set_time(void*);
static void menu_set_date(void*);
static void menu_set_time_mode(void*);
static void menu_set_lcd_brightness(void*);
static void menu_set_lcd_contrast(void*);
static void menu_set_lcd_backlight(void*);
static void menu_reset_temperature(void*);
static void menu_set_temp_disp_time(void*);
static void menu_set_time_disp_time(void*);
static void menu_set_nameday_disp_time(void*);
static void menu_set_wow_disp_time(void*);


struct menu_item items[] = {
	{ "Set Time", MENU_ITEM_DEFAULT, { menu_set_time } }, 
	{ "Set Date", MENU_ITEM_DEFAULT, { menu_set_date } }, 
	{ "Time Mode", MENU_ITEM_DEFAULT, { menu_set_time_mode } }, 
	{ "LCD Brightness", MENU_ITEM_DEFAULT, { menu_set_lcd_brightness } }, 
	{ "LCD Contrast", MENU_ITEM_DEFAULT, { menu_set_lcd_contrast } },
	{ "LCD Backlight", MENU_ITEM_DEFAULT, { menu_set_lcd_backlight } }, 
	{ "Reset temperature", MENU_ITEM_DEFAULT, { menu_reset_temperature } },
	{ "Temp Disp Time", MENU_ITEM_DEFAULT, { menu_set_temp_disp_time } },
	{ "Time Disp Time", MENU_ITEM_DEFAULT, { menu_set_time_disp_time } },
	{ "Nameday Disp Time", MENU_ITEM_DEFAULT, { menu_set_nameday_disp_time } },
	{ "WoW Disp Time", MENU_ITEM_DEFAULT, { menu_set_wow_disp_time } }
};


struct menu g_main_menu = {
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


static void menu_set_time(void *a_data) {
}


static void menu_set_date(void *a_data) {
}


static void menu_set_time_mode(void *a_data) {
}


static void menu_set_lcd_brightness(void *a_data) {
}


static void menu_set_lcd_contrast(void *a_data) {
}


static void menu_set_lcd_backlight(void *a_data) {
}


static void menu_reset_temperature(void *a_data) {
}


static void menu_set_temp_disp_time(void *a_data) {
}


static void menu_set_time_disp_time(void *a_data) {
}


static void menu_set_nameday_disp_time(void *a_data) {
}


static void menu_set_wow_disp_time(void *a_data) {
}



