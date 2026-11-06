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
#include <libopencm3/stm32/timer.h>

#include "config.h"
#include "util.h"
#include "usb_serial.h"
#include "jitter_timer.h"

#define MASK(c) (3<<(2*(c)))

#define INPUT_CAPTURE_PIN (6)
#define OUTPUT_PIN (0)

//#define F_CPU_MEASURED (47838000)
#define F_CPU_MEASURED (F_CPU)

static void tim3_pa6_input_capture_enable(void)
{
    RCC_APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3_CR1 = TIM_CR1_CKD_CK_INT | /* no clock division */
	           TIM_CR1_CMS_EDGE   |
	           TIM_CR1_DIR_UP     |
	           (0*TIM_CR1_CEN)    ; /* don't start counting yet */

	TIM3_CCMR1 = TIM_CCMR1_CC1S_IN_TI1     | /* select input compare 1 register */
	             TIM_CCMR1_IC1F_CK_INT_N_4 | /* 4 cycles input filter           */
	             TIM_CCMR1_IC1PSC_OFF      ; /* capture every pulse             */

	TIM3_CCER = (0*TIM_CCER_CC1P) | /* polarity: raising */
	            (0*TIM_CCER_CC1NP);

	TIM3_CCER |= (1*TIM_CCER_CC1E); /* input capture enable */

}

const char *hex = "0123456789abcdef";

static char buf[64];
static int ix = 0;

static void out_putchar(int c)
{
	buf[ix++] = c;
	if ( (ix == sizeof(buf)) || (c == '\n') )
	{
		if (usb_serial_can_write())
			usb_serial_write_noblock((uint8_t *)buf, ix);
		ix = 0;
	}
}

/*

static void print_hex32(uint32_t x)
{
	unsigned int i;

	for(i=0; i<sizeof(buf); i++)
	{
		out_putchar( hex[(x>>28)& 0xf] & 0xff );
		x<<=4;
	}
}

static void print_hex8(uint8_t x)
{
	out_putchar( hex[(x>>4) & 0xf] );
	out_putchar( hex[(x   ) & 0xf] );
}

static void print_histogram(void)
{
	print("----\r\n");
	int i;
	for (i=1; i<HISTOGRAM_MAX+1; i++)
		if (histogram[i])
		{
			print_hex8(i);
			print(": ");
			print_hex32(histogram[i]);
			print("\r\n");
		}
}

*/

static void clear_histogram(void)
{
	int i;
	for (i=0; i<HISTOGRAM_MAX+1; i++)
		histogram[i]=0;
}

static void print(const char *s)
{
	while (*s)
		out_putchar(*s++);
}

static void print_32(uint32_t x)
{
	uint32_t i=1000000000;

	while (x < i)
	{
//		out_putchar(' ');
		i/=10;
	}
	if (i==0)
		i = 1;

	while (i)
	{
		uint32_t d = x/i;
		out_putchar( hex[d] );
		x-=d*i;
		i/=10;
	}
}

uint16_t frame[FRAME_LEN];

static void frame_set_value(int index, int v)
{
	frame[index] = (v&0xff) | VALUE;
}

static void frame_set_route4(int index, int route, int timeout)
{
	frame[index] = (timeout<<3)| ((1<<route)&7) | ROUTING4;
}

static void frame_set_route5(int index, int route, int timeout)
{
	frame[index] = (timeout<<4)| ((1<<route)&15) | ROUTING5;
}

static void frame_set_rgb(int index, int r, int g, int b)
{
	frame_set_value(index+0, g);
	frame_set_value(index+1, r);
	frame_set_value(index+2, b);
}

const uint8_t fade[] =
{
/*
from math import pi, cos
max_val=0x30
gamma=2.2
factor = max_val / (255.**gamma)
gamma_map = [ int(x**gamma * factor) for x in range(256) ]
wave = [ gamma_map[int(-cos(x*pi/64)*127.5+127.5)] for x in range(128) ]
for i in range(0, 256, 16):
    print ( '\t'+' '.join('0x{:02x},'.format(x+1) for x in wave[i:i+16]) )
*/
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x05, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
	0x0b, 0x0c, 0x0d, 0x0f, 0x10, 0x11, 0x13, 0x14, 0x16, 0x17, 0x19, 0x1b, 0x1c, 0x1e, 0x1f, 0x21,
	0x22, 0x24, 0x25, 0x26, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x2f, 0x30, 0x30, 0x30,
	0x31, 0x30, 0x30, 0x30, 0x2f, 0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x26, 0x25, 0x24,
	0x22, 0x21, 0x1f, 0x1e, 0x1c, 0x1b, 0x19, 0x17, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0f, 0x0d, 0x0c,
	0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x05, 0x04, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
};

static void init_frame(void)
{
	int i;
	for (i=0; i<FRAME_LEN; i++)
		frame_set_value(i, 0x55);

}

static void set_frame_info(int pin, int phase)
{
	frame_set_route5(ROUTE5_PACKET, pin, 15);
	frame_set_rgb(LED_HIDDEN, fade[phase], 0, fade[phase]);
	frame_set_rgb(LED0, fade[phase], fade[phase], fade[phase]);

	switch (pin)
	{
		case 0:
			frame_set_route4(PACKET1, 0, 0);
			frame_set_rgb(LED1, fade[phase], 0, 0);

			frame_set_route4(PACKET2, 1, 0);
			frame_set_rgb(LED2, 0, fade[phase], 0);

			frame_set_route4(PACKET3, 2, 15);         /* sense pin last */
			frame_set_rgb(LED3, 0, 0, fade[phase]);
			break;
		case 1:
			frame_set_route4(PACKET1, 0, 0);
			frame_set_rgb(LED1, fade[phase], 0, 0);

			frame_set_route4(PACKET3, 1, 15);         /* sense pin last */
			frame_set_rgb(LED3, 0, fade[phase], 0);

			frame_set_route4(PACKET2, 2, 0);
			frame_set_rgb(LED2, 0, 0, fade[phase]);
			break;
		case 2:
			frame_set_route4(PACKET3, 0, 15);         /* sense pin last */
			frame_set_rgb(LED3, fade[phase], 0, 0);

			frame_set_route4(PACKET1, 1, 0);
			frame_set_rgb(LED1, 0, fade[phase], 0);

			frame_set_route4(PACKET2, 2, 0);
			frame_set_rgb(LED2, 0, 0, fade[phase]);
			break;
		case 3:
			/* sense pin is on incoming stream */

			frame_set_route4(PACKET1, 0, 0);
			frame_set_rgb(LED1, fade[phase], 0, 0);

			frame_set_route4(PACKET2, 1, 0);
			frame_set_rgb(LED2, 0, fade[phase], 0);

			frame_set_route4(PACKET3, 2, 15);
			frame_set_rgb(LED3, 0, 0, fade[phase]);
			break;
		default:
			break;
	}
}


volatile uint32_t tick=0,last_tick=0;
void SysTick_Handler(void)
{
	tick+=1;
}
 
static void wait(void)
{
	while (tick == last_tick)
		usb_serial_poll();
	last_tick = tick;
}

static void init(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	remap_usb_pins();

	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO0);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO0);
   	gpio_set_af(GPIOA, GPIO_AF1, GPIO6);
   	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO6);

	tim3_pa6_input_capture_enable();
	usb_serial_init();
    enable_sys_tick(F_SYS_TICK_CLK/60);
}

int main(void)
{
	init();
	init_frame();
	TIM3_CR1 |= TIM_CR1_CEN;

	while(!usb_serial_can_write())
		usb_serial_poll();

	for(;;)
	{
		int pin, phase;
		for (pin=0; pin<4; pin++)
		{
			uint32_t max_freq = 0, min_freq = UINT32_MAX;
			for (phase=0; phase<128; phase++)
			{
				set_frame_info(pin, phase);
				clear_histogram();
				bitbang(frame, 0, GPIOA, TIM3);
				wait();

				uint32_t cycles11 = histogram[65]+histogram[66]+histogram[67];
				cycles11 += histogram[64]+histogram[68]; /* probably not needed */
				uint32_t cycles12 = histogram[71]+histogram[72]+histogram[73];
				cycles12 += histogram[70]+histogram[74]; /* probably not needed */
				if (cycles11 + cycles12 == MEASURE_PERIODS)
				{
					uint32_t freq = F_CPU_MEASURED/2000;
					freq *= 11*cycles11+12*cycles12;
					freq /= MEASURE_NATIVE_CYCLES;
					freq *= 2000;
					if (min_freq > freq)
						min_freq = freq;
					if (max_freq < freq)
						max_freq = freq;
				}
			}

			print("pin = ");
			print_32(pin);
			if (max_freq == 0)
			{
				print(" -");
			}
			else
			{
				print(", min = ");
				print_32(min_freq);
				print(", max = ");
				print_32(max_freq);
			}
			print(";\r\n");
		}
	}
}
