#ifndef KEYMATRIX_H
#define KEYMATRIX_H

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

/* Requirements: - all rows are on the same port,
 *               - all columns are on the same port
 * (These ports may or not be the same)
 */

/*

#define KEYMATRIX_ROWS_PORT (GPIOA)
#define KEYMATRIX_ROWS(X) X(4) X(5) X(6) X(7)

#define KEYMATRIX_COLUMNS_PORT (GPIOA)
#define KEYMATRIX_COLUMNS(X) X(0) X(1) X(2) X(3)

*/

/* dirty X-macro tricks */

#define _KEYMATRIX_ARGS_COUNT(n) 1 +
#define KEYMATRIX_N_ROWS ( KEYMATRIX_ROWS(_KEYMATRIX_ARGS_COUNT) 0 )
#define KEYMATRIX_N_COLUMNS ( KEYMATRIX_COLUMNS(_KEYMATRIX_ARGS_COUNT) 0 )
#define KEYMATRIX_N_KEYS ( KEYMATRIX_N_ROWS * KEYMATRIX_N_COLUMNS )

/* */

#ifndef DEBOUNCE_COUNTDOWN
#define DEBOUNCE_COUNTDOWN (20) // milliseconds
#endif

enum
{
	KEY_UP = 0,
	KEY_DOWN,
};

int keymatrix_state(uint32_t key);
void keymatrix_poll(uint32_t time);
void keymatrix_init(void);

/* to be implemented by user */
void keymatrix_up(int key);
void keymatrix_down(int key);

#endif // KEYMATRIX_H
