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
#include "keymatrix.h"
#include "usb_hid_keypad.h"
#include "hid_keydef.h"

/* For possible keys, see ../common/hid_keydef.h */

static const uint32_t keys[KEYMATRIX_N_KEYS] =
{
	KEY_PLAY,                KEY_MUTE,            KEY_VOLUME_DOWN,         KEY_VOLUME_UP,
	KEY_SCAN_PREVIOUS_TRACK, KEY_SCAN_NEXT_TRACK, KEY_DECREASE_BRIGHTNESS, KEY_INCREASE_BRIGHTNESS,
	KEY_TOGGLE_FULL_SCREEN,  KEY_ZOOM_OUT,        KEY_ZOOM_IN,             KEY_CALCULATOR,
	KEY('A'),                KEY('B'),            KEY('C'),                KEY('D'),
};

volatile uint32_t tick=0;
void SysTick_Handler(void)
{
	tick+=1;
}

static void init(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);

	remap_usb_pins();
	keymatrix_init();
	usb_hid_keypad_init(keys, KEYMATRIX_N_KEYS);
	enable_sys_tick(F_SYS_TICK_CLK/1000);
}

void keymatrix_down(int key_index)
{
	usb_hid_keypad_key_down(keys[key_index]);
}

void keymatrix_up(int key_index)
{
	usb_hid_keypad_key_up(keys[key_index]);
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

