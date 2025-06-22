/*
 * Copyright (c) 2024 Erik Bosman <erik@minemu.org>
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
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "config.h"
#include "util.h"
#include "usb_serial.h"


#define BAUDRATE (1500000UL)

#define UART (USART1)
#define UART_RX_PORT (GPIOB)
#define UART_RX_PIN  (GPIO7)

#define UART_TX_PORT (GPIOB)
#define UART_TX_PIN  (GPIO6)

#define UART_DMA (DMA1)
#define DMA_CHANNEL_RX (DMA_CHANNEL3)
#define DMA_CHANNEL_TX (DMA_CHANNEL2)

#define DMA_ISR_TCIF_TX (DMA_ISR_TCIF2)

#define DMA_CONFIG_RX ( DMA_CCR_MINC | DMA_CCR_CIRC | \
                        DMA_CCR_MSIZE_8BIT | DMA_CCR_PSIZE_8BIT )

#define DMA_CONFIG_TX ( DMA_CCR_MINC | \
                        DMA_CCR_DIR | \
                        DMA_CCR_MSIZE_8BIT | DMA_CCR_PSIZE_8BIT | \
                        DMA_CCR_TCIE )

#define RECV_BUF_SZ (256)
uint8_t buf_uart_rx[RECV_BUF_SZ];

static void uart_rx_init(void)
{
	DMA_CPAR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)&USART_RDR(UART);
	DMA_CMAR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)&buf_uart_rx;
	DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX) = sizeof(buf_uart_rx);
	DMA_CCR(UART_DMA, DMA_CHANNEL_RX) = DMA_CONFIG_RX;
	USART_CR3(UART) |= USART_CR3_DMAR;
	gpio_mode_setup(UART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_RX_PIN);
	gpio_set_af(UART_RX_PORT, GPIO_AF0, UART_RX_PIN);
}

static uint32_t rx_cur = 0, rx_end = 0;
static int uart_getchar(void)
{

	if (rx_cur < rx_end)
		return buf_uart_rx[rx_cur++];

	if (rx_cur == RECV_BUF_SZ)
		rx_cur = 0;

	rx_end = RECV_BUF_SZ - DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX);

	if (rx_end < rx_cur)
		rx_end = RECV_BUF_SZ;

	if (rx_cur < rx_end)
		return buf_uart_rx[rx_cur++];
	else
		return -1;

}

static void uart_rx_flush(void)
{
	rx_end = RECV_BUF_SZ - DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX);
	rx_cur = rx_end;
}

static void uart_tx_init(void)
{
	DMA_CPAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)&USART_TDR(UART);
//	DMA_CMAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)&buf;
//	DMA_CNDTR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)size;
	DMA_CCR(UART_DMA, DMA_CHANNEL_TX) = DMA_CONFIG_TX;
	USART_CR3(UART) |= USART_CR3_DMAT;
	gpio_mode_setup(UART_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_TX_PIN);
	gpio_set_af(UART_TX_PORT, GPIO_AF0, UART_TX_PIN);
	nvic_enable_irq(NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ);
	nvic_set_priority(NVIC_DMA1_CHANNEL2_3_DMA2_CHANNEL1_2_IRQ, 1);
}   

static void uart_enable(void)
{   
	USART_CR1(UART) |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

static void uart_set_baudrate(uint32_t baudrate)
{
	uint32_t baudrate_prescale = F_CPU/baudrate;

	if (baudrate_prescale < 0x10)
	{
		USART_CR1(UART) |= USART_CR1_OVER8;
		USART_BRR(UART) = baudrate_prescale+(baudrate_prescale&~7);
	}
	else
	{
		USART_CR1(UART) &=~ USART_CR1_OVER8;
		USART_BRR(UART) = baudrate_prescale;
	}
}

static void uart_init(void)
{
	gpio_set_output_options(UART_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MED, UART_TX_PIN);
	uart_set_baudrate(BAUDRATE);
	uart_rx_init();
	uart_tx_init();
	DMA_CCR(UART_DMA, DMA_CHANNEL_RX) |= DMA_CCR_EN;
	uart_enable();
}

volatile uint32_t tx_busy=0;

void dma1_channel2_3_dma2_channel1_2_isr(void)
{   
	DMA_CCR(UART_DMA, DMA_CHANNEL_TX) &=~ DMA_CCR_EN;
	DMA_IFCR(UART_DMA) = DMA_ISR_TCIF_TX;
	USART_ICR(UART) = USART_ICR_TCCF;
	tx_busy = 0;
}

static void uart_write(uint8_t *buf, size_t len)
{
	DMA_CMAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)buf;
	DMA_CNDTR(UART_DMA, DMA_CHANNEL_TX) = len;
	if (len)
		DMA_CCR(UART_DMA, DMA_CHANNEL_TX) |= DMA_CCR_EN;

	tx_busy = 1;
}

/*
 *
 */

typedef struct __attribute__((packed))
{
	uint16_t g;
	uint16_t r;
	uint16_t b;

} led_t;

#define LED_SIZE (sizeof(led_t))

#define END_MARKER_SIZE (4)
#define END_MARKER "\xff\xff\xff\xf0"

#define N_LEDS_PER_SEGMENT (44)
#define N_SEGMENTS (6)

#define SEGMENT_SIZE (N_LEDS_PER_SEGMENT*LED_SIZE)

#define N_LEDS (N_LEDS_PER_SEGMENT*N_SEGMENTS)
#define FRAME_SIZE (N_LEDS*LED_SIZE+END_MARKER_SIZE)



uint8_t out_frame[FRAME_SIZE];
uint32_t out_frame_len;
uint8_t in_frame[FRAME_SIZE+1];
uint32_t in_frame_ix;

const uint16_t wave[] =
{

/*
from math import pi, cos
max_val=0xff00
gamma=2.8
wave = [ int(max_val*((-cos(x*pi/85)+1.)/2)**gamma) for x in range(170) ] + [ 0 ] * (256-170)
for i in range(0, 256, 8):
    print ( '\t'+' '.join('0x{:04x},'.format(x) for x in wave[i:i+8]) )
*/
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0001, 0x0002, 0x0004, 0x0008, 0x000d, 0x0015, 0x001f, 0x002e,
	0x0041, 0x005a, 0x007b, 0x00a5, 0x00d9, 0x011a, 0x0169, 0x01c9,
	0x023b, 0x02c3, 0x0362, 0x041c, 0x04f3, 0x05ea, 0x0703, 0x0843,
	0x09ab, 0x0b3f, 0x0d01, 0x0ef4, 0x111b, 0x1379, 0x160e, 0x18df,
	0x1beb, 0x1f36, 0x22c0, 0x268a, 0x2a95, 0x2ee1, 0x336d, 0x383a,
	0x3d45, 0x428f, 0x4814, 0x4dd2, 0x53c7, 0x59ee, 0x6045, 0x66c7,
	0x6d6f, 0x7439, 0x7b1f, 0x821a, 0x8926, 0x903b, 0x9752, 0x9e65,
	0xa56d, 0xac62, 0xb33d, 0xb9f7, 0xc087, 0xc6e7, 0xcd0f, 0xd2f8,
	0xd89b, 0xddf1, 0xe2f4, 0xe79e, 0xebe8, 0xefce, 0xf34b, 0xf65a,
	0xf8f7, 0xfb1f, 0xfcd0, 0xfe06, 0xfec1, 0xff00, 0xfec1, 0xfe06,
	0xfcd0, 0xfb1f, 0xf8f7, 0xf65a, 0xf34b, 0xefce, 0xebe8, 0xe79e,
	0xe2f4, 0xddf1, 0xd89b, 0xd2f8, 0xcd0f, 0xc6e7, 0xc087, 0xb9f7,
	0xb33d, 0xac62, 0xa56d, 0x9e65, 0x9752, 0x903b, 0x8926, 0x821a,
	0x7b1f, 0x7439, 0x6d6f, 0x66c7, 0x6045, 0x59ee, 0x53c7, 0x4dd2,
	0x4814, 0x428f, 0x3d45, 0x383a, 0x336d, 0x2ee1, 0x2a95, 0x268a,
	0x22c0, 0x1f36, 0x1beb, 0x18df, 0x160e, 0x1379, 0x111b, 0x0ef4,
	0x0d01, 0x0b3f, 0x09ab, 0x0843, 0x0703, 0x05ea, 0x04f3, 0x041c,
	0x0362, 0x02c3, 0x023b, 0x01c9, 0x0169, 0x011a, 0x00d9, 0x00a5,
	0x007b, 0x005a, 0x0041, 0x002e, 0x001f, 0x0015, 0x000d, 0x0008,
	0x0004, 0x0002, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

uint32_t iter = 0;


static void fill_out_frame(int n_segments, int error)
{
	uint32_t v = iter++;

	if (n_segments < 0 || n_segments > N_SEGMENTS)
		n_segments = N_SEGMENTS;

	led_t *fb = (led_t *)out_frame;

	int n_leds = n_segments*N_LEDS_PER_SEGMENT;

	for (int i=0; i<n_leds; i++)
	{
		fb[i].r = wave[ ( i*3 + iter ) & 0xff ];
		if (!error)
		{
			fb[i].g = wave[ ( i*3 + iter*5 +  85 ) & 0xff ];
			fb[i].b = wave[ ( i*3 + iter*5 + 170 ) & 0xff ];
		}
		else
		{
			fb[i].g = 0;
			fb[i].b = 0;
		}
	}

	int len = n_leds * LED_SIZE;

	memcpy(&out_frame[len], END_MARKER, END_MARKER_SIZE);

	out_frame_len = len+END_MARKER_SIZE;
}


static uint32_t ff_count;
static void clear_in_frame(void)
{
	in_frame_ix=0;
	ff_count = 0;
	memset(in_frame, 0, sizeof(in_frame));
}

static int in_poll(void)
{
	int c = uart_getchar();

	if (c == -1)
		return 0;

	if (in_frame_ix < sizeof(in_frame))
		in_frame[in_frame_ix++] = c;

	if (ff_count == 3 && c == 0xf0)
		return 1;

	if (c == 0xff)
		ff_count += 1;
	else
		ff_count = 0;

	return 0;
}

/*
 *
 */

volatile uint32_t tick=0;
void SysTick_Handler(void)
{
	tick+=1;
}

static void gpio_write_mask(uint32_t gpioport, uint16_t gpios, uint16_t mask)
{
	GPIO_BSRR(gpioport) = ( (mask &~ gpios) << 16) | (mask & gpios);
}

static void init(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_DMA1);

	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO0);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO4|GPIO5|GPIO6|GPIO7);

	gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO8);

    remap_usb_pins();
    usb_serial_init();
    uart_init();

	enable_sys_tick(F_SYS_TICK_CLK/60);
}


static int button_down(void)
{
	return gpio_get(GPIOA, GPIO0) == 0;
}

static int dfu_button_down(void)
{
	return gpio_get(GPIOB, GPIO8) != 0;
}

static void led_status(int error)
{
	gpio_write_mask(GPIOA, error ? (GPIO4|GPIO5|GPIO6) : 0, GPIO4|GPIO5|GPIO6|GPIO7);
}

static void usb_print(const char *s)
{
	usb_serial_write_noblock((uint8_t *)s, strlen(s));
}

#define N_ITERS (120)

static int send_recv_test(void)
{
	int i, error=0, res=0;

	uint32_t t;

	int n=N_SEGMENTS, n_min=N_SEGMENTS;
	for (i=0; i<N_ITERS; i++)
	{
		fill_out_frame(n, ( i== N_ITERS-1 ) && res );
		clear_in_frame();

		t=tick;
		while(t==tick);
		t=tick;

		uart_rx_flush();
		uart_write(out_frame, out_frame_len);

		while ( !in_poll() && t==tick );

		int n_data = in_frame_ix-END_MARKER_SIZE;

		if ( (n_data % SEGMENT_SIZE) == 0 )
		{
			n_min = N_SEGMENTS - n_data / SEGMENT_SIZE;
			error = memcmp(&out_frame[out_frame_len-in_frame_ix], in_frame, in_frame_ix);
		}
		else
			error = 1;

		if (error)
			res = 1;

		usb_serial_poll();
		while(tx_busy);

		n--;
		if (n<n_min)
			n=N_SEGMENTS;

		led_status(error);
	}

	return res;
}


static int recv_test(void)
{
	uint32_t t;
	t=tick;
	uart_rx_flush();
	clear_in_frame();

	while ( !in_poll() )
		if ( tick - t > 10 )
			return 1;

	clear_in_frame();

	while ( !in_poll() )
		if ( tick - t > 10 )
			return 1;

	int n_data = in_frame_ix-END_MARKER_SIZE;
	int error = (n_data % SEGMENT_SIZE) != 0;
	return error;
}

int main(void)
{
	int error=0;

	init();

	usb_serial_poll();

	for(;;)
	{

		if (button_down())
			error = send_recv_test();
		if (dfu_button_down())
			error = recv_test();

		usb_print( error ? "error\n" : "ok\n" );
		led_status(error);
		usb_serial_poll();

	}
}

