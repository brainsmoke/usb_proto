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

#include "keymatrix.h"

#define CLEAR(x) ((x)<<16)
#define SET(x) (x)

#define BIT(n) (1<<n)
#define OR_BIT(n) BIT(n) |
#define SELECT_ROW(n) ( SET(BIT(n)) | CLEAR(rows_mask^BIT(n)) ) ,
#define BIT_LIST(n) BIT(n) ,

static const uint16_t rows_mask = ( KEYMATRIX_ROWS(OR_BIT) 0 );
static const uint16_t columns_mask = ( KEYMATRIX_COLUMNS(OR_BIT) 0 );
static const uint32_t select_row[] = { KEYMATRIX_ROWS(SELECT_ROW) };
static const uint16_t column_lookup[KEYMATRIX_N_COLUMNS] = { KEYMATRIX_COLUMNS(BIT_LIST) };

static uint16_t debounce[KEYMATRIX_N_KEYS];
static uint16_t status[KEYMATRIX_N_KEYS];
static int row;

int keymatrix_state(uint32_t key)
{
	if (key >= KEYMATRIX_N_KEYS)
		return -1;
	return status[key];
}

static void keymatrix_next_row(void)
{
	row += 1;
	if (row >= KEYMATRIX_N_ROWS)
		row=0;
	GPIO_BSRR(KEYMATRIX_ROWS_PORT) = select_row[row];
}

static void keymatrix_read_column(void)
{
	int col;
	uint16_t port = gpio_get(KEYMATRIX_COLUMNS_PORT, columns_mask);
	if (port)
	for (col=0; col<KEYMATRIX_N_COLUMNS; col++)
	{
		int key = col*KEYMATRIX_N_ROWS+row;

		if (debounce[key])
			continue;

		int pressed = (port & column_lookup[col]);

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
void keymatrix_poll(uint32_t time)
{
	static enum
	{
		PHASE_SELECT,
		PHASE_READ,

	} phase = PHASE_SELECT;

	int key;
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

void keymatrix_init(void)
{
	memset(debounce, 0, sizeof(debounce));
	memset(status, 0, sizeof(status));
	gpio_mode_setup(KEYMATRIX_ROWS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, rows_mask);
	gpio_mode_setup(KEYMATRIX_COLUMNS_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, columns_mask);
	row = KEYMATRIX_N_ROWS-1;
}

