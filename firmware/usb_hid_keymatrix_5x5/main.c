/*
 * Copyright (c) 2023-2024 Erik Bosman <erik@minemu.org>
 *
 * Permission  is  hereby  granted,  free  of  charge,  to  any  person
 * obtaining  a copy  of  this  software  and  associated documentation
 * files (the "Software"),  to deal in the Software without restriction,
 * including  without  limitation  the  rights  to  use,  copy,  modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the
 * Software,  and to permit persons to whom the Software is furnished to
 * do so, subject to the following conditions:
 *
 * The  above  copyright  notice  and this  permission  notice  shall be
 * included  in  all  copies  or  substantial portions  of the Software.
 *
 * THE SOFTWARE  IS  PROVIDED  "AS IS", WITHOUT WARRANTY  OF ANY KIND,
 * EXPRESS OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,  FITNESS  FOR  A  PARTICULAR  PURPOSE  AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT,  TORT OR OTHERWISE,  ARISING FROM, OUT OF OR IN
 * CONNECTION  WITH THE SOFTWARE  OR THE USE  OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * (http://opensource.org/licenses/mit-license.html)
 *
 */

#include <stdint.h>
#include <string.h>

#include <libopencmsis/core_cm3.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "config.h"
#include "util.h"
#include "usb_hid_keypad.h"

#ifndef DEBOUNCE_COUNTDOWN
#define DEBOUNCE_COUNTDOWN (20) // milliseconds
#endif

enum
{
	KEY_UP = 0,
	KEY_DOWN,
};

/*
 * Specific to a single wiring configuration and a bit more messy than keymatrix.c but with more addressable keys.
 *
 */

#define ROWS_PORT (GPIOA)
#define ROWS_MASK (0xf8)

#define COLUMNS_PORT_A (GPIOA)
#define COLUMNS_MASK_A (0x7)
#define COLUMNS_LSHIFT_A (2)

#define COLUMNS_PORT_B (GPIOB)
#define COLUMNS_MASK_B (0xC0)
#define COLUMNS_RSHIFT_B (6)

#define CLEAR(x) ((x)<<16)
#define SET(x) (x)

#define BIT(n) (1ul<<n)
#define SELECT_ROW(n) ( SET(BIT(n)) | CLEAR(ROWS_MASK^BIT(n)) )

static const uint32_t select_row[] =
{
	SELECT_ROW(7),
	SELECT_ROW(6),
	SELECT_ROW(5),
	SELECT_ROW(4),
	SELECT_ROW(3),
};

#define KEYMATRIX_N_COLUMNS (5ul)
#define KEYMATRIX_N_ROWS (sizeof(select_row)/sizeof(select_row[0]))
#define KEYMATRIX_N_KEYS (KEYMATRIX_N_ROWS*KEYMATRIX_N_COLUMNS)


static uint16_t debounce[KEYMATRIX_N_KEYS];
static uint16_t status[KEYMATRIX_N_KEYS];
static uint32_t row;

static const uint32_t keys[KEYMATRIX_N_KEYS] =
{
	KEYS
};

static void keymatrix_down(int key_index)
{
	usb_hid_keypad_key_down(keys[key_index]);
}

static void keymatrix_up(int key_index)
{
	usb_hid_keypad_key_up(keys[key_index]);
}

static void keymatrix_next_row(void)
{
	row += 1;
	if (row >= KEYMATRIX_N_ROWS)
		row=0;
	GPIO_BSRR(ROWS_PORT) = select_row[row];
}

static void keymatrix_read_column(void)
{
	uint32_t col;
	uint16_t col_bits = ( gpio_get(COLUMNS_PORT_A, COLUMNS_MASK_A)<<COLUMNS_LSHIFT_A ) |
	                    ( gpio_get(COLUMNS_PORT_B, COLUMNS_MASK_B)>>COLUMNS_RSHIFT_B ) ;

	for (col=0; col<KEYMATRIX_N_COLUMNS; col++)
	{
		int key = col+row*KEYMATRIX_N_ROWS;

		if (debounce[key])
			continue;

		int pressed = (col_bits & BIT(col));

		if (pressed && status[key] == KEY_UP)
		{
			keymatrix_down(key);
			status[key] = KEY_DOWN;
			debounce[key] = DEBOUNCE_COUNTDOWN;
		}
		if (!pressed && status[key] != KEY_UP)
		{
			keymatrix_up(key);
			status[key] = KEY_UP;
			debounce[key] = DEBOUNCE_COUNTDOWN;
		}
	}
}

static uint32_t last_time=0;
static void keymatrix_poll(uint32_t time)
{
	static enum
	{
		PHASE_SELECT,
		PHASE_READ,

	} phase = PHASE_SELECT;

	uint32_t key;
	if (last_time != time)
		for(key=0; key<KEYMATRIX_N_KEYS; key++)
			if (debounce[key])
				debounce[key]--;
	last_time = time;

	if ( phase == PHASE_SELECT )
	{
		keymatrix_next_row();
		phase = PHASE_READ;
	}
	else
	{
		keymatrix_read_column();
		phase = PHASE_SELECT;
	}
}

static void keymatrix_init(void)
{
	memset(debounce, 0, sizeof(debounce));
	memset(status, 0, sizeof(status));
	gpio_mode_setup(ROWS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ROWS_MASK);

	gpio_mode_setup(COLUMNS_PORT_A, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, COLUMNS_MASK_A);
	gpio_mode_setup(COLUMNS_PORT_B, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, COLUMNS_MASK_B);
	row = KEYMATRIX_N_ROWS-1;
}


volatile uint32_t tick=0;
void SysTick_Handler(void)
{
	tick+=1;
}

static void init(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	remap_usb_pins();
	keymatrix_init();
	usb_hid_keypad_init(keys, KEYMATRIX_N_KEYS);
	enable_sys_tick(F_SYS_TICK_CLK/1000);
}

int main(void)
{
	init();

	for(;;)
	{
		keymatrix_poll(tick);
		usb_hid_keypad_poll();
	}
}


