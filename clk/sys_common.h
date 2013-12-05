#ifndef __SYS_COMMON_H__
#define __SYS_COMMON_H__

#include "pca.h"


void bus_ow_setup(struct soft_ow *a_sow_ctx);
void bus_twi_setup(struct twi_ctx **a_ctx);


#endif /* __SYS_COMMON_H__ */
