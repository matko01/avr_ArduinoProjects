#ifndef PORT_H_YGLBOTDE
#define PORT_H_YGLBOTDE

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


/**
 * @brief port description
 */
struct t_pin {
	volatile unsigned char *port;
	unsigned char pin;
};

#define PIN_SET(__pin) \
	*(__pin)->port |= (unsigned char)_BV((__pin)->pin)

#define PIN_CLEAR(__pin) \
	*(__pin)->port &= ~(unsigned char)_BV((__pin)->pin)

#define PIN_SET_VALUE(__pin, __value) \
	if (__value) \
		PIN_SET(__pin); \
	else \
		PIN_CLEAR(__pin);

void pin_conf_output(volatile struct t_pin *a_pin);


#endif /* end of include guard: PORT_H_YGLBOTDE */

