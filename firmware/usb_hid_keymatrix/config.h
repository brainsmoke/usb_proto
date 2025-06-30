#ifndef CONFIG_H
#define CONFIG_H

#include <libopencm3/stm32/gpio.h>

#define MANUFACTURER_STRING "example co."
#define PRODUCT_STRING "usb hid demo"
#define SERIAL_STRING "00000001"

#define F_CPU (48000000)
#define F_SYS_TICK_CLK (F_CPU/8)

#define KEYMATRIX_ROWS_PORT (GPIOA)
#define KEYMATRIX_ROWS(X) X(4) X(5) X(6) X(7)

#define KEYMATRIX_COLUMNS_PORT (GPIOA)
#define KEYMATRIX_COLUMNS(X) X(0) X(1) X(2) X(3)


#endif // CONFIG_H
