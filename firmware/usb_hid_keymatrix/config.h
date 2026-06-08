#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "example co."
#define PRODUCT_STRING "usb hid demo"
#define SERIAL_STRING "00000001"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#define KEYMATRIX_ROWS_PORT (GPIOA)
#define KEYMATRIX_ROWS(X) X(7) X(6) X(5) X(4)

#define KEYMATRIX_COLUMNS_PORT (GPIOA)
#define KEYMATRIX_COLUMNS(X) X(0) X(1) X(2) X(3)

#define KEYS \
	KEY_PLAY,                KEY_MUTE,            KEY_VOLUME_DOWN,         KEY_VOLUME_UP, \
	KEY_SCAN_PREVIOUS_TRACK, KEY_SCAN_NEXT_TRACK, KEY_DECREASE_BRIGHTNESS, KEY_INCREASE_BRIGHTNESS, \
	KEY_TOGGLE_FULL_SCREEN,  KEY_ZOOM_OUT,        KEY_ZOOM_IN,             KEY_CALCULATOR, \
	KEY('A'),                KEY('B'),            KEY('C'),                KEY('D'), \

#endif // CONFIG_H
