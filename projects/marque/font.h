#ifndef FONT_H_04HYE1VX
#define FONT_H_04HYE1VX

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

#ifdef FONT_5X4

#define FONT_5X4_MAX_WIDTH 5

extern const unsigned char* const font_5x4[64];
unsigned char print_char(const unsigned char a_char, unsigned char *a_buffer, unsigned char a_count);
#endif


#endif /* end of include guard: FONT_H_04HYE1VX */

