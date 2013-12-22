#ifndef __LCD_H__
#define __LCD_H__

#include "sys_conf.h"
#include "pca.h"

#define LCD_CHARACTERS_PER_LINE 16
#define LCD_NUMBER_OF_LINES 2

#define LCD_LINE00_ADDR 0x00
#define LCD_LINE01_ADDR 0x11
#define LCD_LINE10_ADDR 0x40
#define LCD_LINE11_ADDR 0x51

void lcd_setup(struct dev_hd44780_ctx *a_lcd_ctx);
void lcd_blit(uint8_t which);

#endif /* __LCD_H__ */
