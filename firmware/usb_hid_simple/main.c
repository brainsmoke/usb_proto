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

#define N_KEYS (10)

#include "config.h"
#include "util.h"
#include "usb_hid_keypad.h"
#include "hid_keydef.h"

/* For possible keys, see ../common/hid_keydef.h */

static const uint32_t keys[N_KEYS] =
{
	KEY('A'),
	KEY('B'),
	KEY('C'),
	KEY('D'),
	KEY('E'),
	KEY('F'),
	KEY('G'),
	KEY('H'),
	KEY('I'),
	KEY('J'),
};

static uint32_t keystate = 0;

#define DEBOUNCE (20) // ms

#define GPIOA_MASK (0xff)
#define GPIOB_MASK ((1<<6)|(1<<7))

static uint32_t keys_debounce[N_KEYS] = { 0, };

volatile uint32_t tick=0, last_tick=0;
void SysTick_Handler(void)
{
	tick+=1;
}

static void init(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIOA_MASK);
	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIOB_MASK);

	remap_usb_pins();
	usb_hid_keypad_init(keys, N_KEYS);
	enable_sys_tick(F_SYS_TICK_CLK/1000);
}

static void set_keystate(int key_index, int state)
{
	if (keys_debounce[key_index])
		return;

	if ( !(keystate & (1<<key_index)) && state)
	{
		usb_hid_keypad_key_down(keys[key_index]);
		keys_debounce[key_index] = DEBOUNCE;
		keystate |= (1<<key_index);
	}

	if ( (keystate & (1<<key_index)) && !state)
	{
		usb_hid_keypad_key_up(keys[key_index]);
		keys_debounce[key_index] = DEBOUNCE;
		keystate &=~ (1<<key_index);
	}
}

static void keys_poll(void)
{
	int i;
	if (tick != last_tick)
		for (i=0; i<N_KEYS; i++)
			if (keys_debounce[i])
				keys_debounce[i]--;
	last_tick = tick;

	uint32_t porta = gpio_get(GPIOA, GPIOA_MASK);
	uint32_t portb = gpio_get(GPIOB, GPIOB_MASK);

	for (i=0; i<8; i++)
		set_keystate(7-i, !(porta & (1<<i)));

	set_keystate(8, !(portb & (1<<7)));
	set_keystate(9, !(portb & (1<<6)));
}

int main(void)
{
	init();

	for(;;)
	{
		keys_poll();
		usb_hid_keypad_poll();
	}
}

