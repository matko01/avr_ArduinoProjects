#ifndef __SETUP_H__
#define __SETUP_H__

#include "pca.h"

void lcd_setup(struct dev_hd44780_ctx *a_lcd_ctx);
void thermo_setup(struct soft_ow *a_sow_ctx);

#endif /* __SETUP_H__ */
