#ifndef KEYPAD_H
#define KEYPAD_H

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

#include <libopencm3/stm32/gpio.h>

#include "config.h"


#define PORT_KEY_ROWS (GPIOA)

#define PIN_KEY_ROW_0 (GPIO4)
#define PIN_KEY_ROW_1 (GPIO5)
#define PIN_KEY_ROW_2 (GPIO6)
#define PIN_KEY_ROW_3 (GPIO7)

#define PORT_KEY_COLUMNS (GPIOA)

#define PIN_KEY_COLUMN_0 (GPIO0)
#define PIN_KEY_COLUMN_1 (GPIO1)
#define PIN_KEY_COLUMN_2 (GPIO2)
#define PIN_KEY_COLUMN_3 (GPIO3)

#define N_COLUMNS (4)
#define N_ROWS (4)
#define N_KEYS (N_ROWS*N_COLUMNS)

#define MASK_KEY_ROWS (PIN_KEY_ROW_0|PIN_KEY_ROW_1|PIN_KEY_ROW_2|PIN_KEY_ROW_3)
#define SELECT_ROW(n) ( PIN_KEY_ROW_##n | (MASK_KEY_ROWS^PIN_KEY_ROW_##n)<<16 )
#define SELECT_ROWS { SELECT_ROW(0), SELECT_ROW(1), SELECT_ROW(2), SELECT_ROW(3) }

#define MASK_KEY_COLUMNS (PIN_KEY_COLUMN_0|PIN_KEY_COLUMN_1|PIN_KEY_COLUMN_2|PIN_KEY_COLUMN_3)
#define COLUMN_LOOKUP { PIN_KEY_COLUMN_0, PIN_KEY_COLUMN_1, PIN_KEY_COLUMN_2, PIN_KEY_COLUMN_3 }


#ifndef DEBOUNCE_COUNTDOWN
#define DEBOUNCE_COUNTDOWN (20) // milliseconds
#endif

enum
{
	KEY_UP = 0,
	KEY_DOWN,
};

int keypad_state(uint32_t key);
void keypad_poll(uint32_t time);
void keypad_init(void);

/* to be implemented by user */
void keypad_up(int key);
void keypad_down(int key);

#endif // KEYPAD_H
