#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "example co."
#define PRODUCT_STRING "usb hid demo"
#define SERIAL_STRING "00000001"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#include "hid_keydef.h"

/* For possible keys, see ../common/hid_keydef.h */

#define KEYS \
	KEY('A'), KEY('B'), KEY('C'), KEY('D'), KEY('E'), \
	KEY('F'), KEY('G'), KEY('H'), KEY('I'), KEY('J'), \
	KEY('K'), KEY('L'), KEY('M'), KEY('N'), KEY('O'), \
	KEY('P'), KEY('Q'), KEY('R'), KEY('S'), KEY('T'), \
	KEY('U'), KEY('V'), KEY('W'), KEY('X'), KEY('Y'),

#endif // CONFIG_H
