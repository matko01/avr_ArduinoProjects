#ifndef __MARQUEE_H__
#define __MARQUEE_H__

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


#include "config.h"
#include "display.h"

/// buffer length
#define BUFFER_LENGTH 250

/// buffer padding
#define BUFFER_PADDING 5

/// total buffer size
#define BUFFER_SIZE (BUFFER_LENGTH + BUFFER_PADDING)

/**
 * @brief marque buffer data type
 */
typedef struct _t_marque_buffer {
	unsigned char data[BUFFER_SIZE];
	unsigned char screen[DISPLAY_WIDTH];
	unsigned int head;
	unsigned int current;
} t_marque_buffer;

/**
 * @brief marque data type
 */
typedef struct _t_marque {
	volatile t_marque_buffer *buff;
	unsigned int delay;
} t_marque;


void marque_init();
void marque_clear();
void marque_scroll();

void marque_print(const char* a_str);
void marque_append_char(unsigned char);
void marque_set_delay(unsigned int a_delay);

volatile t_marque* marque_get_ctx();

#endif /* __MARQUEE_H__ */
