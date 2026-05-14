#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "Spacebar"
#define PRODUCT_STRING "Spacebar"
#define SERIAL_STRING "12345678"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#include "hid_keydef.h"

#define KEYS \
	KEY(' '),

#endif // CONFIG_H
