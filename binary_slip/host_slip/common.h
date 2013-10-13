#ifndef COMMON_H_YNA7GV18
#define COMMON_H_YNA7GV18

#include <stdint.h>

#define SLIP_CHAR_SEND(__x)	slip_sendc(__x)
#define SLIP_CHAR_RECV(__x) slip_getc(__x)

unsigned int slip_sendc(unsigned char a_char);
unsigned char slip_getc(unsigned char *a_data);

#endif /* COMMON_H_YNA7GV18 */

