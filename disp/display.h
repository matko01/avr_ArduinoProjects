#ifndef __DISPLAY_H__
#define __DISPLAY_H__

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


e_return display_install(t_display_dev *a_dev);
e_return display_set(t_display a_disp);
e_return display_blit(t_display a_disp);

void display_putpixel(t_display a_disp, unsigned char a_pos);
void display_clear(t_display a_disp);
void display_fill(t_display a_disp);


#endif /* __DISPLAY_H__ */
