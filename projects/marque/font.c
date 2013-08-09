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

#include "font.h"
#include <avr/pgmspace.h>

#ifdef FONT_5X4

/* 
 * 5-high FONT FOR RENDERING TO THE LED SCREEN.
 * Includes kerning support
 * Gaurav Manek, 2011
 */

#define FONT_5X4_HEIGHT  5
#define FONT_5X4_STEP_GLYPH 10
#define FONT_5X4_ASCII_OFFSET 32
// Number of bytes per glyph 

const unsigned char font_5x4_width [] PROGMEM = {
  1, 1, 3, 5, 4, 5, 4, 1,
  2, 2, 3, 5, 2, 3, 1, 4,
  3, 3, 4, 3, 4, 3, 4, 4,
  4, 4, 1, 2, 3, 3, 3, 4,
  5, 4, 3, 4, 3, 3, 3, 4,
  4, 3, 3, 4, 3, 5, 5, 4,
  3, 5, 3, 4, 3, 4, 5, 5,
  3, 3, 4, 2, 4, 2, 3, 3
};

const unsigned char FONT_5X4_CHAR0[] PROGMEM = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 }; //   SPACE
const unsigned char FONT_5X4_CHAR1[] PROGMEM = { 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0b10000000 }; //   !
const unsigned char FONT_5X4_CHAR2[] PROGMEM = { 0b10100000, 0b10100000, 0b00000000, 0b00000000, 0b00000000 }; //   "
const unsigned char FONT_5X4_CHAR3[] PROGMEM = { 0b01010000, 0b11111000, 0b01010000, 0b11111000, 0b01010000 }; //   #
const unsigned char FONT_5X4_CHAR4[] PROGMEM = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 }; //   $
const unsigned char FONT_5X4_CHAR5[] PROGMEM = { 0b11001000, 0b11010000, 0b00100000, 0b01011000, 0b10011000 }; //   %
const unsigned char FONT_5X4_CHAR6[] PROGMEM = { 0b01100000, 0b10010000, 0b01000000, 0b10110000, 0b01110000 }; //   &
const unsigned char FONT_5X4_CHAR7[] PROGMEM = { 0b10000000, 0b10000000, 0b00000000, 0b00000000, 0b00000000 }; //   '
const unsigned char FONT_5X4_CHAR8[] PROGMEM = { 0b01000000, 0b10000000, 0b10000000, 0b10000000, 0b01000000 }; //   (
const unsigned char FONT_5X4_CHAR9[] PROGMEM = { 0b10000000, 0b01000000, 0b01000000, 0b01000000, 0b10000000 }; //   )
const unsigned char FONT_5X4_CHAR10[] PROGMEM = { 0b10100000, 0b01000000, 0b10100000, 0b00000000, 0b00000000 }; //   *
const unsigned char FONT_5X4_CHAR11[] PROGMEM = { 0b00100000, 0b00100000, 0b11111000, 0b00100000, 0b00100000 }; //   +
const unsigned char FONT_5X4_CHAR12[] PROGMEM = { 0b00000000, 0b00000000, 0b00000000, 0b01000000, 0b10000000 }; //   ,
const unsigned char FONT_5X4_CHAR13[] PROGMEM = { 0b00000000, 0b00000000, 0b11100000, 0b00000000, 0b00000000 }; //   -
const unsigned char FONT_5X4_CHAR14[] PROGMEM = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000 }; //   .
const unsigned char FONT_5X4_CHAR15[] PROGMEM = { 0b00010000, 0b00100000, 0b01100000, 0b01000000, 0b10000000 }; //   /
const unsigned char FONT_5X4_CHAR16[] PROGMEM = { 0b01000000, 0b10100000, 0b10100000, 0b10100000, 0b01000000 }; //   0
const unsigned char FONT_5X4_CHAR17[] PROGMEM = { 0b01000000, 0b11000000, 0b01000000, 0b01000000, 0b11100000 }; //   1
const unsigned char FONT_5X4_CHAR18[] PROGMEM = { 0b01100000, 0b10010000, 0b00100000, 0b01000000, 0b11110000 }; //   2
const unsigned char FONT_5X4_CHAR19[] PROGMEM = { 0b11000000, 0b00100000, 0b11000000, 0b00100000, 0b11000000 }; //   3
const unsigned char FONT_5X4_CHAR20[] PROGMEM = { 0b00100000, 0b01100000, 0b10100000, 0b11110000, 0b00100000 }; //   4
const unsigned char FONT_5X4_CHAR21[] PROGMEM = { 0b11100000, 0b10000000, 0b11100000, 0b00100000, 0b11000000 }; //   5
const unsigned char FONT_5X4_CHAR22[] PROGMEM = { 0b01100000, 0b10000000, 0b11100000, 0b10010000, 0b01100000 }; //   6
const unsigned char FONT_5X4_CHAR23[] PROGMEM = { 0b11110000, 0b00010000, 0b01100000, 0b01000000, 0b01000000 }; //   7
const unsigned char FONT_5X4_CHAR24[] PROGMEM = { 0b01100000, 0b10010000, 0b01100000, 0b10010000, 0b01100000 }; //   8
const unsigned char FONT_5X4_CHAR25[] PROGMEM = { 0b01100000, 0b10010000, 0b01110000, 0b00010000, 0b00100000 }; //   9
const unsigned char FONT_5X4_CHAR26[] PROGMEM = { 0b00000000, 0b10000000, 0b00000000, 0b10000000, 0b00000000 }; //   :
const unsigned char FONT_5X4_CHAR27[] PROGMEM = { 0b00000000, 0b01000000, 0b00000000, 0b01000000, 0b10000000 }; //   ;
const unsigned char FONT_5X4_CHAR28[] PROGMEM = { 0b00100000, 0b01000000, 0b10000000, 0b01000000, 0b00100000 }; //   <
const unsigned char FONT_5X4_CHAR29[] PROGMEM = { 0b00000000, 0b11100000, 0b00000000, 0b11100000, 0b00000000 }; //   =
const unsigned char FONT_5X4_CHAR30[] PROGMEM = { 0b10000000, 0b01000000, 0b00100000, 0b01000000, 0b10000000 }; //   >
const unsigned char FONT_5X4_CHAR31[] PROGMEM = { 0b01100000, 0b10010000, 0b00010000, 0b00100000, 0b00100000 }; //   ?
const unsigned char FONT_5X4_CHAR32[] PROGMEM = { 0b01111000, 0b10001000, 0b10111000, 0b10101000, 0b00010000 }; //   @
const unsigned char FONT_5X4_CHAR33[] PROGMEM = { 0b01100000, 0b10010000, 0b11110000, 0b10010000, 0b10010000 }; //   A
const unsigned char FONT_5X4_CHAR34[] PROGMEM = { 0b11000000, 0b10100000, 0b11000000, 0b10100000, 0b11000000 }; //   B
const unsigned char FONT_5X4_CHAR35[] PROGMEM = { 0b01100000, 0b10010000, 0b10000000, 0b10010000, 0b01100000 }; //   C
const unsigned char FONT_5X4_CHAR36[] PROGMEM = { 0b11000000, 0b10100000, 0b10100000, 0b10100000, 0b11000000 }; //   D
const unsigned char FONT_5X4_CHAR37[] PROGMEM = { 0b11100000, 0b10000000, 0b11000000, 0b10000000, 0b11100000 }; //   E
const unsigned char FONT_5X4_CHAR38[] PROGMEM = { 0b11100000, 0b10000000, 0b11100000, 0b10000000, 0b10000000 }; //   F
const unsigned char FONT_5X4_CHAR39[] PROGMEM = { 0b01100000, 0b10010000, 0b10000000, 0b10110000, 0b01100000 }; //   G
const unsigned char FONT_5X4_CHAR40[] PROGMEM = { 0b10010000, 0b10010000, 0b11110000, 0b10010000, 0b10010000 }; //   H
const unsigned char FONT_5X4_CHAR41[] PROGMEM = { 0b11100000, 0b01000000, 0b01000000, 0b01000000, 0b11100000 }; //   I
const unsigned char FONT_5X4_CHAR42[] PROGMEM = { 0b11100000, 0b00100000, 0b00100000, 0b10100000, 0b01000000 }; //   J
const unsigned char FONT_5X4_CHAR43[] PROGMEM = { 0b10010000, 0b10100000, 0b11000000, 0b10100000, 0b10010000 }; //   K
const unsigned char FONT_5X4_CHAR44[] PROGMEM = { 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b11100000 }; //   L
const unsigned char FONT_5X4_CHAR45[] PROGMEM = { 0b10001000, 0b11011000, 0b10101000, 0b10001000, 0b10001000 }; //   M
const unsigned char FONT_5X4_CHAR46[] PROGMEM = { 0b10001000, 0b11001000, 0b10101000, 0b10011000, 0b10001000 }; //   N
const unsigned char FONT_5X4_CHAR47[] PROGMEM = { 0b01100000, 0b10010000, 0b10010000, 0b10010000, 0b01100000 }; //   O
const unsigned char FONT_5X4_CHAR48[] PROGMEM = { 0b11000000, 0b10100000, 0b11000000, 0b10000000, 0b10000000 }; //   P
const unsigned char FONT_5X4_CHAR49[] PROGMEM = { 0b01110000, 0b10001000, 0b10001000, 0b10010000, 0b01101000 }; //   Q
const unsigned char FONT_5X4_CHAR50[] PROGMEM = { 0b11000000, 0b10100000, 0b11000000, 0b10100000, 0b10100000 }; //   R
const unsigned char FONT_5X4_CHAR51[] PROGMEM = { 0b01110000, 0b10000000, 0b01100000, 0b00010000, 0b11100000 }; //   S
const unsigned char FONT_5X4_CHAR52[] PROGMEM = { 0b11100000, 0b01000000, 0b01000000, 0b01000000, 0b01000000 }; //   T
const unsigned char FONT_5X4_CHAR53[] PROGMEM = { 0b10010000, 0b10010000, 0b10010000, 0b10010000, 0b01100000 }; //   U
const unsigned char FONT_5X4_CHAR54[] PROGMEM = { 0b10001000, 0b10001000, 0b01010000, 0b01010000, 0b00100000 }; //   V
const unsigned char FONT_5X4_CHAR55[] PROGMEM = { 0b10001000, 0b10001000, 0b10101000, 0b10101000, 0b01010000 }; //   W
const unsigned char FONT_5X4_CHAR56[] PROGMEM = { 0b10100000, 0b10100000, 0b01000000, 0b10100000, 0b10100000 }; //   X
const unsigned char FONT_5X4_CHAR57[] PROGMEM = { 0b10100000, 0b10100000, 0b01000000, 0b01000000, 0b01000000 }; //   Y
const unsigned char FONT_5X4_CHAR58[] PROGMEM = { 0b11110000, 0b00010000, 0b01100000, 0b10000000, 0b11110000 }; //   Z
const unsigned char FONT_5X4_CHAR59[] PROGMEM = { 0b11000000, 0b10000000, 0b10000000, 0b10000000, 0b11000000 }; //   [
const unsigned char FONT_5X4_CHAR60[] PROGMEM = { 0b10000000, 0b01000000, 0b01100000, 0b00100000, 0b00010000 }; //   backslash
const unsigned char FONT_5X4_CHAR61[] PROGMEM = { 0b11000000, 0b01000000, 0b01000000, 0b01000000, 0b11000000 }; //   ]
const unsigned char FONT_5X4_CHAR62[] PROGMEM = { 0b01000000, 0b10100000, 0b00000000, 0b00000000, 0b00000000 }; //   ^
const unsigned char FONT_5X4_CHAR63[] PROGMEM = { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11100000 }; //   _


/**
 * @brief font declaration
 */
const unsigned char* const font_5x4[64] PROGMEM = {
   FONT_5X4_CHAR0,
   FONT_5X4_CHAR1,
   FONT_5X4_CHAR2,
   FONT_5X4_CHAR3,
   FONT_5X4_CHAR4,
   FONT_5X4_CHAR5,
   FONT_5X4_CHAR6,
   FONT_5X4_CHAR7,
   FONT_5X4_CHAR8,
   FONT_5X4_CHAR9,
   FONT_5X4_CHAR10,
   FONT_5X4_CHAR11,
   FONT_5X4_CHAR12,
   FONT_5X4_CHAR13,
   FONT_5X4_CHAR14,
   FONT_5X4_CHAR15,
   FONT_5X4_CHAR16,
   FONT_5X4_CHAR17,
   FONT_5X4_CHAR18,
   FONT_5X4_CHAR19,
   FONT_5X4_CHAR20,
   FONT_5X4_CHAR21,
   FONT_5X4_CHAR22,
   FONT_5X4_CHAR23,
   FONT_5X4_CHAR24,
   FONT_5X4_CHAR25,
   FONT_5X4_CHAR26,
   FONT_5X4_CHAR27,
   FONT_5X4_CHAR28,
   FONT_5X4_CHAR29,
   FONT_5X4_CHAR30,
   FONT_5X4_CHAR31,
   FONT_5X4_CHAR32,
   FONT_5X4_CHAR33,
   FONT_5X4_CHAR34,
   FONT_5X4_CHAR35,
   FONT_5X4_CHAR36,
   FONT_5X4_CHAR37,
   FONT_5X4_CHAR38,
   FONT_5X4_CHAR39,
   FONT_5X4_CHAR40,
   FONT_5X4_CHAR41,
   FONT_5X4_CHAR42,
   FONT_5X4_CHAR43,
   FONT_5X4_CHAR44,
   FONT_5X4_CHAR45,
   FONT_5X4_CHAR46,
   FONT_5X4_CHAR47,
   FONT_5X4_CHAR48,
   FONT_5X4_CHAR49,
   FONT_5X4_CHAR50,
   FONT_5X4_CHAR51,
   FONT_5X4_CHAR52,
   FONT_5X4_CHAR53,
   FONT_5X4_CHAR54,
   FONT_5X4_CHAR55,
   FONT_5X4_CHAR56,
   FONT_5X4_CHAR57,
   FONT_5X4_CHAR58,
   FONT_5X4_CHAR59,
   FONT_5X4_CHAR60,
   FONT_5X4_CHAR61,
   FONT_5X4_CHAR62,
   FONT_5X4_CHAR63
};

unsigned char print_char(const unsigned char a_char, unsigned char *a_buffer, unsigned char a_count) {

	unsigned char len = 0x00;
	unsigned char tmp[5] = {0x00};
	unsigned char idx = (a_char - FONT_5X4_ASCII_OFFSET);

	if ((a_char - FONT_5X4_ASCII_OFFSET) > 63)
		return 0;

	len = pgm_read_byte(&(font_5x4_width[idx]));

	if (len > a_count)
		len = a_count;

	memcpy_P(tmp, (PGM_P)pgm_read_word(&font_5x4[idx]), 5);

	for (unsigned char i = 0; i < len; i++) {
		for (unsigned char j = 0; j < 5; j++) {
			if (tmp[j] & (0x80 >> i))
				a_buffer[i] |= (0x10 >> j);
		}
	}

	return len;
}

#endif 

