#include "menu.h"

#include <stdio.h>
#include <stdlib.h>


struct menu_item items[] = {
	{ "opcja 1", MENU_ITEM_DEFAULT, NULL }, 
	{ "opcja 2", MENU_ITEM_DEFAULT, NULL }, 
	{ "opcja 3", MENU_ITEM_DEFAULT, NULL }, 
	{ "opcja 4", MENU_ITEM_DEFAULT, NULL }, 
	{ "opcja 5", MENU_ITEM_DEFAULT, NULL }
};


struct menu main_menu = {
	.items = items,
	.parent = NULL,
	.cnt = MENU_ITEMS_SIZE(items)
};



int main(int argc, char const *argv[])
{
	char buff[80] = "";
	menu_render(&main_menu, 0, buff, 32);
	printf("%s\n", buff);
	return 0;
}
