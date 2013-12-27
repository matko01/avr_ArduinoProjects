#ifndef TMP_CTX_H_INZKGQ4V
#define TMP_CTX_H_INZKGQ4V

#include <stdint.h>
#include "pca.h"

/**
 * @brief defines number of Timer overflow interrupts 
 * needed to finish the temperature measurement
 */
#define TEMPERATURE_MEASUREMENT_INTERVAL 52


/**
 * @brief temperature measurement steps
 */
typedef enum _temp_msr_state_t {
	TEMP_MEASUREMENT_IDLE = 0,
	TEMP_MEASUREMENT_TRIGGERED,
	TEMP_MEASUREMENT_TIMEOUT
} temp_msr_state_t;


/**
 * @brief temperature measurement ctx
 */
struct temp_msr_ctx {
	// time interval
	volatile uint16_t tv;

	// temperature measurement fsm state
	volatile uint8_t state;

	// measurements
	volatile uint16_t temp;	
	volatile uint16_t temp_max;
	volatile uint16_t temp_min;
};


struct temp_ctx {
	struct temp_msr_ctx msr;
	volatile struct soft_ow *sow_ctx;
};


void tmp_reset(volatile struct temp_msr_ctx *msr);
uint8_t tmp_setup(volatile struct temp_ctx *a_ctx);
void tmp_update_tv(volatile struct temp_msr_ctx *a_ctx);
void tmp_update_measurements(volatile struct twi_ctx *a_twi, volatile struct temp_ctx *a_ctx);
void tmp_trigger_measurement(volatile struct temp_ctx *a_ctx);


#endif /* TMP_CTX_H_INZKGQ4V */

