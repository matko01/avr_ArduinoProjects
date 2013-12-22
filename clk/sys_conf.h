#ifndef SYS_CONFIG_H_B5FPFCAM
#define SYS_CONFIG_H_B5FPFCAM


// I2C address of the TWI device
#define TWI_RTC_ADDR 0x68

#define RTC_MAGIC_OFFSET (DS1307_CONTROL_ADDR + 1)
#define MAX_TEMP_ADDR (RTC_MAGIC_OFFSET + 1)
#define MIN_TEMP_ADDR (MAX_TEMP_ADDR + 2)

// one wire thermometer setup
#define OW_PORT PORTB
#define OW_PIN 4


// lcd io lines setup
#define LCD_DB_PORT PORTD
#define LCD_DB_PIN_FIRST 4
#define LCD_RS_PORT PORTB
#define LCD_RS_PIN 1
#define LCD_E_PORT PORTB
#define LCD_E_PIN 2

#define RTC_LED_PIN 5
#define RTC_LED_PORT PORTB

#endif /* SYS_CONFIG_H_B5FPFCAM */

