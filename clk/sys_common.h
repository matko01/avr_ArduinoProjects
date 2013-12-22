#ifndef __SYS_COMMON_H__
#define __SYS_COMMON_H__

#include "pca.h"
#include "sys_ctx.h"

#define SET_CONTRAST(__contrast) OCR2B = __contrast
#define SET_BRIGHTNESS(__brightness) OCR2A = __brightness

// ================================================================================

/**
 * @brief setup system timers
 */
void timers_setup(void);

/**
 * @brief initialize the RTC hardware and it's settings
 *
 * @param a_ctx system context
 */
void rtc_setup(volatile struct twi_ctx *a_ctx);

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

// ================================================================================

/**
 * @brief restore system settings from EEPROM
 *
 * @param a_ss system settings structure
 */
void sys_settings_get(struct sys_settings *a_ss);

// ================================================================================

void displayTemp(void);
void displayTime(void);
void displayNameday(void);
void displayProverb(void);
void displayMenu(void);
void displayClean(uint8_t a_which);

// ================================================================================

uint8_t is_leap_year(uint8_t a_year);
uint16_t get_year_day(void);
void blit(uint8_t which);

// ================================================================================

#endif /* __SYS_COMMON_H__ */
