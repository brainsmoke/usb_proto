#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "example co."
#define PRODUCT_STRING "usb hid demo"
#define SERIAL_STRING "00000001"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#include "hid_keydef.h"

#define KEYS \
    KEY('A'),        \
    KEY('B'),        \
    KEY('X'),        \
    KEY('Y'),        \
    KEY_RETURN,      \
    KEY_SPACEBAR,    \
    KEY_RIGHT_ARROW, \
    KEY_UP_ARROW,    \
    KEY_DOWN_ARROW,  \
    KEY_LEFT_ARROW,

#endif // CONFIG_H
