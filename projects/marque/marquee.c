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

#include <string.h>
#include <util/delay.h>

#include "marquee.h"
#include "font.h"

/// marque context declaration
static volatile t_marque g_marque;

/// marque buffer declaration
static volatile t_marque_buffer g_marque_buffer;


/**
 * @brief initialization
 */
void marque_init() {
	marque_clear();
	display_set((volatile t_display)g_marque_buffer.screen);

	g_marque.buff = &g_marque_buffer;
	g_marque.delay = 0x3000;
}


/**
 * @brief clear the buffer
 */
void marque_clear() {
	memset((void *)&g_marque_buffer, 0x00, sizeof(g_marque_buffer));
}


/**
 * @brief print an ascii string to marque buffer
 *
 * @param a_str
 */
void marque_print(const char* a_str) {
	unsigned char i = 0;
	for (unsigned char j = 0; j < strlen(a_str); j++) {
		display_clear((volatile t_display)&g_marque_buffer.data[g_marque_buffer.head]);
		
		i = print_char(a_str[j],
			   	(volatile t_display)&g_marque_buffer.data[g_marque_buffer.head],
			   	FONT_5X4_MAX_WIDTH);

		g_marque_buffer.head += i;
		g_marque_buffer.head = (g_marque_buffer.head + 1) % BUFFER_LENGTH;
	}
}

/**
 * @brief configure scolling delay
 *
 * @param a_delay
 */
void marque_set_delay(unsigned int a_delay) {
	 g_marque.delay = a_delay;
}


/**
 * @brief scroll marque by one
 */
void marque_scroll() {
	display_clear((volatile t_display)g_marque_buffer.screen);
	display_blit((volatile t_display)&g_marque_buffer.data[ (g_marque_buffer.current++) % (g_marque_buffer.head + 1)]);

	for (unsigned char i = 0; i < 32; i++) {
		_delay_loop_2(g_marque.delay);
	}
}

/**
 * @brief get marque context data
 *
 * @return t_marque pointer
 */
volatile t_marque* marque_get_ctx() {
	return &g_marque;
}
