#ifndef __DISPLAY_H__
#define __DISPLAY_H__

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

#include "port.h"
#include "common.h"

#define DISPLAY_HEIGHT 5
#define DISPLAY_WIDTH 5

/// combine pixel data into a byte
#define MAKE_COORD(__x, __y, __mark) \
	(unsigned char)( ((__x & 0x07) << 5) | ((__y & 0x07) << 2) | (__mark & 0x01))

/// extract x coordinate out of the data compound
#define GET_X(__pos) \
	(unsigned char)((__pos >> 5) & 0x07)

/// extract y coordinate out of the data compound
#define GET_Y(__pos) \
	(unsigned char)((__pos >> 2) & 0x07)

/// extract pixel marking out of the data compound
#define GET_MARK(__pos) \
	(unsigned char)(__pos & 0x01)

/**
 * @brief defines screen buffer
 */
typedef unsigned char* t_display;

/**
 * @brief defines display device connection
 */
typedef struct _t_display_dev {
	struct t_pin rows[DISPLAY_HEIGHT];
	struct t_pin cols[DISPLAY_WIDTH];
} t_display_dev;


e_return display_install(volatile t_display_dev *a_dev);
e_return display_set(volatile t_display a_disp);
e_return display_blit(volatile t_display a_disp);

void display_putpixel(t_display a_disp, unsigned char a_pos);
void display_clear(t_display a_disp);
void display_fill(t_display a_disp);


#endif /* __DISPLAY_H__ */
