#include "sys_ctx.h"
#include "scroll_string.h"

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

	if (!(a_cnt % 24) && !a_scrl_str->_f) {
		a_scrl_str->pos = (a_scrl_str->pos + 1) % (a_scrl_str->len + del_len);
		a_scrl_str->_f = 1;
	}
	else if (a_cnt % 24) {
		a_scrl_str->_f = 0;
	}
}
