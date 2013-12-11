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
	volatile uint16_t tv;
	volatile uint8_t state;
	volatile uint16_t temp;
	volatile uint16_t temp_max;
	volatile uint16_t temp_min;
};


uint8_t tmp_setup(volatile struct temp_msr_ctx *a_ctx, volatile struct soft_ow *a_bus);
void tmp_update_tv(volatile struct temp_msr_ctx *a_ctx);
void tmp_update_measurements(volatile struct temp_msr_ctx *a_ctx, volatile struct soft_ow *a_bus);
void tmp_trigger_measurement(volatile struct temp_msr_ctx *a_ctx, volatile struct soft_ow *a_bus);


#endif /* TMP_CTX_H_INZKGQ4V */

