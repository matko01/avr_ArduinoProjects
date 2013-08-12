#ifndef TDELAY_H_IPOQKAD0
#define TDELAY_H_IPOQKAD0

#define TDELAY_IMPLEMENT_T0_INT 1
#define TDELAY_IMPLEMENT_T1_INT 1
#define TDELAY_IMPLEMENT_T2_INT 1

#include "common.h"
#include "tdelay_common.h"


void tdelay_init(e_timer a_timer);
void tdelay_ms(e_timer, uint32_t a_delay);


#endif /* end of include guard: TDELAY_H_IPOQKAD0 */
