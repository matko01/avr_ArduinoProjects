#ifndef __SCROLL_STRING_H__
#define __SCROLL_STRING_H__


struct scroll_str {
	uint8_t pos;
	uint8_t len;
	uint8_t _f;
	
	// must be at the end
	char *s;
};


void scroll_str_init(struct scroll_str *a_scrl_str, char *a_str, uint8_t a_len);
void scroll_str_paste(struct scroll_str *a_scrl_str, char *a_output, uint8_t a_len, volatile uint16_t a_cnt);


#endif /* __SCROLL_STRING_H__ */
