#include "sys_ctx.h"
#include "string_util.h"

#include <string.h>


void scroll_str_init(struct scroll_str *a_scrl_str, char *a_str, uint8_t a_len) {
	a_scrl_str->s = a_str;
	a_scrl_str->len = a_len;
	a_scrl_str->pos = 0;
}


void scroll_str_paste(struct scroll_str *a_scrl_str, char *a_output, uint8_t a_len, volatile uint16_t a_cnt) {
	const char *delimiter = " ** ";
	const uint8_t del_len = strlen(delimiter);

	if (a_scrl_str->len <= a_len) {
		strcpy(a_output, a_scrl_str->s);
	}
	else {
		int16_t remaining_len = a_scrl_str->len - a_scrl_str->pos;

		if (remaining_len <= 0) {
			uint8_t n = del_len - (a_scrl_str->pos - a_scrl_str->len);
			strncpy(a_output, delimiter + (del_len - n), n);
			strncpy(a_output + n, a_scrl_str->s, a_len - n);
		}
		else if (remaining_len < a_len) {
			uint8_t nd = (a_len - remaining_len);
			uint8_t rem_begin = nd;

			nd = nd > del_len ? del_len : nd;
			rem_begin -= nd;

			strncpy(a_output, a_scrl_str->s + a_scrl_str->pos, remaining_len);
			strncpy(a_output + remaining_len, delimiter, nd);

			if ((remaining_len + nd) < a_len) {
				strncpy(a_output + remaining_len + nd, a_scrl_str->s, rem_begin);
			}
		}
		else {
			strncpy(a_output, a_scrl_str->s + a_scrl_str->pos, a_len);
		}
	}

	if (!(a_cnt % SCROLLING_SPEED) && !a_scrl_str->_f) {
		a_scrl_str->pos = (a_scrl_str->pos + 1) % (a_scrl_str->len + del_len);
		a_scrl_str->_f = 1;
	}
	else if (a_cnt % SCROLLING_SPEED) {
		a_scrl_str->_f = 0;
	}
}


void blink_str_init(struct blink_str *a_bl_str, char *a_str, char replacer) {
	a_bl_str->len = strlen(a_str);
	a_bl_str->str = a_str;
	a_bl_str->replace = replacer;	
	a_bl_str->_f = 0;
	a_bl_str->marker = 0;
}


void blink_str_paste(struct blink_str *a_bl_str, char *a_output, uint8_t a_len, volatile uint16_t a_cnt) {

	if (a_bl_str->_f) {
		strncpy(a_output, a_bl_str->str, a_len);
	}
	else {
		for (uint8_t i = 0; i<a_len; i++) {
			a_output[i] = a_bl_str->replace;
		}
	}

	if (a_cnt > a_bl_str->marker) {
		a_bl_str->marker = a_cnt + BLINKING_SPEED;
		a_bl_str->_f = (a_bl_str->_f + 1) % 2;
	}
}
