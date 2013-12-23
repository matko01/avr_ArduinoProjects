#ifndef __SYS_SETUP_H__
#define __SYS_SETUP_H__

#include "pca.h"

#define SET_CONTRAST(__contrast) OCR2B = __contrast
#define SET_BRIGHTNESS(__brightness) OCR2A = __brightness


/**
 * @brief setup one wire bus
 *
 * @param a_sow_ctx system context
 */
void bus_ow_setup(struct soft_ow *a_sow_ctx);


/**
 * @brief initialize the two wire interface
 *
 * @param a_ctx system context
 */
void bus_twi_setup(struct twi_ctx **a_ctx);


/**
 * @brief setup the GPIO for the "seconds" LED
 */
void led_setup(gpio_pin *led);


/**
 * @brief setup system timers
 */
void timers_setup(void);



#endif /* __SYS_SETUP_H__ */
