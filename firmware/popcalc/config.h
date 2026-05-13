#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "YourBoySerge inc."
#define PRODUCT_STRING "popcalc!"
#define SERIAL_STRING "42424242"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#include "hid_keydef.h"

#define KEYS \
	KEY_CALCULATOR,

#endif // CONFIG_H
