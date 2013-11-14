#ifndef ONE_WIRE_H_Y9ZW2KEM
#define ONE_WIRE_H_Y9ZW2KEM

#include <avr/io.h>
#include <stdint.h>


#define OW_DDR	DDRC
#define OW_PORTO PORTC
#define OW_PORTI PINC
#define OW_PIN  PORTC0


/**
 * @brief enable/disable strong pull-up
 *
 * @param a_enable 1 - enable, 0 - disable
 */
void ow_strong_pullup(uint8_t a_enable);


/**
 * @brief configure the PIN in pull-up high mode
 */
void ow_enable();


/**
 * @brief generate the initialization pulse
 *
 * @return 1 if presence pulse detected, 0 otherwise
 */
uint8_t ow_initialize();


/**
 * @brief write a single bit on the one wire bus
 *
 * @param a_bit
 *
 * @return always 1
 */
uint8_t ow_write_bit(uint8_t a_bit);


/**
 * @brief read a single bit out of the one wire bus
 *
 * @return bit value
 */
uint8_t ow_read_bit();


/**
 * @brief write a whole byte on the one wire bus
 *
 * @param a_byte byte to be written
 *
 * @return always one
 */
uint8_t ow_write_byte(uint8_t a_byte);


/**
 * @brief read a single byte out of one wire bus
 *
 * @return byte read
 */
uint8_t ow_read_byte();


/**
 * @brief write whole data buffer in burst to the one wire bus
 *
 * @param a_data data to be written
 * @param a_len number of bytes
 *
 * @return number of bytes written
 */
uint8_t ow_write_data(uint8_t *a_data, uint8_t a_len);


/**
 * @brief read a whole data burst from the one wire bus
 *
 * @param a_data buffer for the data
 * @param a_maxlen how much data to read
 *
 * @return number of bytes read
 */
uint8_t ow_read_data(uint8_t *a_data, uint8_t a_maxlen);


#endif /* end of include guard: ONE_WIRE_H_Y9ZW2KEM */

