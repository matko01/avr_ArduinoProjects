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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <string.h>

#include <util/delay.h>

#include "display.h"
#include "common.h"

static volatile t_display g_display_memory = 0x00;
static volatile t_display_dev *g_dev = 0x00; 
static volatile unsigned char g_column = 0;

// will be triggered with f = 61Hz
ISR(TIMER0_OVF_vect)
{
	if (g_display_memory) {

		/// disable previous column
		PIN_CLEAR(&(g_dev->cols[g_column]));
		
		/// continue to next column
		g_column++;
	   	g_column = g_column % DISPLAY_WIDTH;

		/// set row pin values
		for (unsigned char i = 0; i<DISPLAY_HEIGHT; i++) {
			PIN_SET_VALUE(&(g_dev->rows[i]), 
					!(g_display_memory[g_column] & (0x01 << i)));
		}

		/// pull it to ground if any rows are turned on
		PIN_SET_VALUE(&(g_dev->cols[g_column]), 
				g_display_memory[g_column]);
	}
}

e_return display_install(volatile t_display_dev* a_dev)
{
	// setup ports
	g_dev = a_dev;

	if ((g_dev = a_dev)) {
		for (unsigned char i = 0; 
				i<MAX(DISPLAY_WIDTH, DISPLAY_HEIGHT); i++) {

			if (i<DISPLAY_WIDTH)
				pin_conf_output(&g_dev->rows[i]);

			if (i<DISPLAY_HEIGHT)
				pin_conf_output(&g_dev->cols[i]);
		}
	}
	else {
		return RET_ERROR_PARAMETER; 
	}

	// enable timer power
	power_timer0_enable();

	// zero the timer
	TCNT0 = 0x00;

	// normal mode, 1024 clk prescaler
	TCCR0A = 0x00;
	TCCR0B = 0x04;

	// enable overflow interrupt, + enable global interrupts
	TIMSK0 = 0x01;
	sei();

	return RET_OK;
}

/**
 * @brief change the address of the framebuffer
 *
 * @param a_disp
 */
e_return display_set(volatile t_display a_disp)
{
	unsigned char cnt = 128;
	e_return ret = RET_ERROR;
	
	while (++cnt) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
			if (g_column == (DISPLAY_WIDTH - 1) ||
					!g_display_memory) {
				g_display_memory = a_disp;
				ret = RET_OK;
				break;
			}
		} // ATOMIC_BLOCK
	} // while

	return ret;
}

e_return display_blit(volatile t_display a_disp)
{
	if (!g_display_memory)
		return RET_ERROR_RESOURCES;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		memcpy(g_display_memory, a_disp, DISPLAY_WIDTH);		
	}

	return RET_OK;
}

void display_clear(t_display a_disp) 
{
	if (!a_disp)
		return;

	memset(a_disp, 0x00, DISPLAY_WIDTH);
}

void display_fill(t_display a_disp)
{
	if (!a_disp)
		return;

	// this should be adjusted if display height > 16
	memset(a_disp, 0xff, DISPLAY_WIDTH);
}


void display_putpixel(t_display a_disp, unsigned char a_data)
{
	if (!a_disp)
		return;

	if (GET_MARK(a_data))
		a_disp[GET_X(a_data)] |= (0x01 << GET_Y(a_data));
	else
		a_disp[GET_Y(a_data)] &= ~(0x01 << GET_Y(a_data));
}
