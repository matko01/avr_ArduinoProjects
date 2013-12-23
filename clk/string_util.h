#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <stdint.h>

#define SCROLLING_SPEED 16
#define BLINKING_SPEED 16


struct scroll_str {
	uint8_t pos;
	uint8_t len;
	uint8_t _f;
	
	// must be at the end
	char *s;
};


struct blink_str {
	// replacement character
	char replace;
	// length of the string
	uint8_t len;
	// timestamp marker
	uint16_t marker;
	// flag
	uint8_t _f;

	// the string itself
	char *str;
};


void scroll_str_init(struct scroll_str *a_scrl_str, char *a_str, uint8_t a_len);
void scroll_str_paste(struct scroll_str *a_scrl_str, char *a_output, uint8_t a_len, volatile uint16_t a_cnt);

void blink_str_init(struct blink_str *a_bl_str, char *a_str, char replacer);
void blink_str_paste(struct blink_str *a_bl_str, char *a_output, uint8_t a_len, volatile uint16_t a_cnt);

#endif /* __STRING_UTIL_H__ */
