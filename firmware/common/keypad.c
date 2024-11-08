/*
 * Copyright (c) 2023 Erik Bosman <erik@minemu.org>
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
#include <libopencm3/stm32/gpio.h>

#include "keypad.h"

static const uint32_t select_row[N_ROWS] = SELECT_ROWS;
static const uint16_t column_lookup[N_COLUMNS] = COLUMN_LOOKUP;
static uint16_t debounce[N_KEYS];
static uint16_t status[N_KEYS];
static int row;

int keypad_state(uint32_t key)
{
	if (key >= N_KEYS)
		return -1;
	return status[key];
}

static void keypad_next_row(void)
{
	row += 1;
	if (row >= N_ROWS)
		row=0;
	GPIO_BSRR(PORT_KEY_ROWS) = select_row[row];
}

static void keypad_read_column(void)
{
	int col;
	uint16_t port = gpio_get(PORT_KEY_COLUMNS, MASK_KEY_COLUMNS);
	for (col=0; col<N_COLUMNS; col++)
	{
		int key = col*N_ROWS+row;

		if (debounce[key])
			continue;

		int pressed = (port & column_lookup[col]);

		if (pressed && status[key] == KEY_UP)
		{
			keypad_down(key);
			status[key] = KEY_DOWN;
			debounce[key] = DEBOUNCE_COUNTDOWN;
		}
		if (!pressed && status[key] != KEY_UP)
		{
			keypad_up(key);
			status[key] = KEY_UP;
			debounce[key] = DEBOUNCE_COUNTDOWN;
		}
	}
}

static uint32_t last_time=0;
void keypad_poll(uint32_t time)
{
	static enum
	{
		PHASE_SELECT,
		PHASE_READ,

	} phase = PHASE_SELECT;

	int key;
	if (last_time != time)
		for(key=0; key<N_KEYS; key++)
			if (debounce[key])
				debounce[key]--;
	last_time = time;

	if ( phase == PHASE_SELECT )
	{
		keypad_next_row();
		phase = PHASE_READ;
	}
	else
	{
		keypad_read_column();
		phase = PHASE_SELECT;
	}
}

void keypad_init(void)
{
	memset(debounce, 0, sizeof(debounce));
	memset(status, 0, sizeof(status));
	gpio_mode_setup(PORT_KEY_ROWS, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MASK_KEY_ROWS);
	gpio_mode_setup(PORT_KEY_COLUMNS, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, MASK_KEY_COLUMNS);
	row = N_ROWS-1;
}

