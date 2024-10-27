#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "example co."
#define PRODUCT_STRING "usb serial demo"
#define SERIAL_STRING "00000001"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#define DEFAULT_BAUDRATE (115200)
#define MIN_BAUDRATE (2400UL) /*?*/
#define MAX_BAUDRATE (4000000UL)

#endif // CONFIG_H
