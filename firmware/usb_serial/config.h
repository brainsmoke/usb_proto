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
#define MAX_LOWSPEED_BAUDRATE (800000UL)

#define PORT_LINE_STATE (GPIOA)

#define PIN_DTR       (GPIO0)
#define PIN_RTS       (GPIO1)
#define PIN_ESP_GPIO0 (GPIO6)
#define PIN_ESP_EN    (GPIO7)

#define LINE_STATE_PIN_MASK (PIN_DTR|PIN_RTS|PIN_ESP_GPIO0|PIN_ESP_EN)
#define LINE_STATE_DEFAULT (LINE_STATE_PIN_MASK)

#endif // CONFIG_H
