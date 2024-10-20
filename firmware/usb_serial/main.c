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
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "config.h"
#include "util.h"
#include "usb_serial.h"

#define PACKET_SIZE (64)
#define OVERFLOW_DEADZONE (128)
#define UART_RX_BUFSIZE (1024+OVERFLOW_DEADZONE)
#define UART_TX_BUFSIZE (1024)

static uint8_t buf_uart_rx[UART_RX_BUFSIZE+PACKET_SIZE];
static uint8_t buf_uart_tx[UART_TX_BUFSIZE+PACKET_SIZE];
static volatile uint32_t off_tx_head, off_tx_tail, off_tx_next_tail, off_tx_end;

#define UART (USART1)
#define UART_RX_PORT (GPIOB)
#define UART_RX_PIN  (GPIO7)

#define UART_TX_PORT (GPIOB)
#define UART_TX_PIN  (GPIO6)

#define UART_DMA (DMA1)
#define DMA_CHANNEL_RX (DMA_CHANNEL2)
#define DMA_CHANNEL_TX (DMA_CHANNEL3)

#define DMA_CONFIG_RX ( DMA_CCR_MINC | DMA_CCR_CIRC | \
                        DMA_CCR_MSIZE_8BIT | DMA_CCR_PSIZE_8BIT )

#define DMA_CONFIG_TX ( DMA_CCR_MINC | \
                        DMA_CCR_DIR | \
                        DMA_CCR_MSIZE_8BIT | DMA_CCR_PSIZE_8BIT | \
                        DMA_CCR_TCIE )

static void uart_rx_init(void)
{
	DMA_CPAR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)&USART_RDR(UART);
	DMA_CMAR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)&buf_uart_rx;
	DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)UART_RX_BUFSIZE;
	DMA_CCR(UART_DMA, DMA_CHANNEL_RX) = DMA_CONFIG_RX;
	USART_CR3(UART) |= USART_CR3_DMAR;
	gpio_mode_setup(UART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_RX_PIN);
	gpio_set_af(UART_RX_PORT, GPIO_AF1, UART_RX_PIN);
}

static void uart_tx_init(void)
{
	DMA_CPAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)&USART_TDR(UART);
//	DMA_CMAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)&buf;
//	DMA_CNDTR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)size;
	DMA_CCR(UART_DMA, DMA_CHANNEL_TX) = DMA_CONFIG_TX;
	USART_CR3(UART) |= USART_CR3_DMAT;
	gpio_mode_setup(UART_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_TX_PIN);
	gpio_set_af(UART_TX_PORT, GPIO_AF1, UART_TX_PIN);

	off_tx_head = off_tx_tail = off_tx_next_tail = off_tx_end = 0;
}

static void uart_init(long baudrate_prescale)
{
	if (baudrate_prescale < 0x10)
	{
		USART_CR1(UART) = USART_CR1_OVER8;
		USART_BRR(UART) = baudrate_prescale+(baudrate_prescale&~7);
	}
	else
	{
		USART_CR1(UART) = 0;
		USART_BRR(UART) = baudrate_prescale;
	}

	uart_rx_init();
	uart_tx_init();
	DMA_CCR(UART_DMA, DMA_CHANNEL_RX) |= DMA_CCR_EN;
	USART_CR1(UART) |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

void dma1_channel2_3_dma2_channel1_2_isr(void)
{
	DMA_CCR(UART_DMA, DMA_CHANNEL_TX) &=~ DMA_CCR_EN;
	DMA_CMAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)&buf_uart_tx[off_tx_next_tail];

	uint32_t size = off_tx_end - off_tx_next_tail;

	if (off_tx_end < UART_TX_BUFSIZE && size > PACKET_SIZE)
		size = PACKET_SIZE;

	DMA_CNDTR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)size;

	if (size)
		DMA_CCR(UART_DMA, DMA_CHANNEL_TX) |= DMA_CCR_EN;

	off_tx_tail = off_tx_next_tail;
	off_tx_next_tail += size;

	if (off_tx_next_tail >= UART_TX_BUFSIZE)
	{
		off_tx_next_tail = 0;
		off_tx_end = off_tx_head;
	}

	DMA_IFCR(UART_DMA) = DMA_ISR_TCIF4;
}

static void uart_tx_restart_dma(void)
{
	dma1_channel2_3_dma2_channel1_2_isr();
}

static void init(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_DMA1);

	remap_usb_pins();
	usb_serial_init();
	uart_init(DEFAULT_BAUDRATE);
}

static uint32_t get_rx_head(void)
{
	return UART_RX_BUFSIZE-1-DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX);
}

int main(void)
{
	init();

//	int overflow = 0;
	uint32_t rx_tail = 0, rx_head = 0;
	for(;;)
	{
		if ( (off_tx_tail - off_tx_head) > PACKET_SIZE )
		{
			uint32_t n = usb_serial_read(&buf_uart_tx[off_tx_head], PACKET_SIZE);

			if (n != 0)
			{
				cm_disable_interrupts();
				uint32_t next_head = off_tx_head + n;

				if (off_tx_end < next_head)
					off_tx_end = next_head;

				if (next_head >= UART_TX_BUFSIZE)
					next_head = 0;

				off_tx_head = next_head;

				if (!( DMA_CCR(UART_DMA, DMA_CHANNEL_TX) & DMA_CCR_EN ) )
				{
					cm_enable_interrupts();
					uart_tx_restart_dma();
				}
				cm_enable_interrupts();
			}
		}

		uint32_t i = rx_head;
		rx_head = get_rx_head();
		for (; i < PACKET_SIZE && i < rx_head; i++)
			buf_uart_rx[i+UART_RX_BUFSIZE] = buf_uart_rx[i];

		uint32_t len = rx_head-rx_tail;
		if (len > UART_RX_BUFSIZE)
			len += UART_RX_BUFSIZE;

		if (len > UART_RX_BUFSIZE-OVERFLOW_DEADZONE)
		{
//			overflow = 1;
			rx_tail = rx_head - OVERFLOW_DEADZONE;
			if (rx_tail > UART_RX_BUFSIZE)
				rx_tail += UART_RX_BUFSIZE;
		}

		if (len > PACKET_SIZE)
			len = PACKET_SIZE;

		rx_tail += usb_serial_write_noblock(&buf_uart_rx[rx_tail], len);

		if ( rx_tail >= UART_RX_BUFSIZE )
			rx_tail -= UART_RX_BUFSIZE;

		usb_serial_poll();
	}
}

