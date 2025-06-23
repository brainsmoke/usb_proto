#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>
#include <libopencmsis/core_cm3.h>

#define MANUFACTURER_STRING "example co."
#define PRODUCT_STRING "driver for obegraensad"
#define SERIAL_STRING "00000001"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)


#endif // CONFIG_H
