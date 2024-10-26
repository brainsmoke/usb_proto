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

#define PACKET_SIZE (64UL)
#define OVERFLOW_DEADZONE (128UL)

#define UART_RX_CAPACITY (1024UL)
#define UART_RX_RINGSIZE (UART_RX_CAPACITY+OVERFLOW_DEADZONE)

#define UART_TX_CAPACITY (1024UL)
#define UART_TX_RINGSIZE (UART_TX_CAPACITY)

/* RX / TX are used in ths file from the point of view of the UART */

/* some modifications have been made the the data structures
 * since our usb serial library is not ringbuffer aware.
 */

/* DMA ringbuffer with an extra packet of padding,
 * up to 64 bytes from the start of the buffer get mirrored
 * on the end.
 */
static uint8_t buf_uart_rx[UART_RX_RINGSIZE+PACKET_SIZE];
static uint8_t buf_uart_tx[UART_TX_RINGSIZE+PACKET_SIZE];

uint32_t rx_head, rx_tail, rx_transfer_size;

volatile uint32_t tx_head, tx_size, tx_transfer_size;
uint32_t tx_tail;

int overflow;

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

static void uart_rx_init(void)
{
	DMA_CPAR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)&USART_RDR(UART);
	DMA_CMAR(UART_DMA, DMA_CHANNEL_RX) = (uint32_t)&buf_uart_rx;
	DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX) = UART_RX_RINGSIZE;
	DMA_CCR(UART_DMA, DMA_CHANNEL_RX) = DMA_CONFIG_RX;
	USART_CR3(UART) |= USART_CR3_DMAR;
	gpio_mode_setup(UART_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, UART_RX_PIN);
	gpio_set_af(UART_RX_PORT, GPIO_AF0, UART_RX_PIN);
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

void wwdg_isr(void)
{
	reset_system();
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
	uart_init(F_CPU/DEFAULT_BAUDRATE);

	rx_head = rx_tail = rx_transfer_size = 0;
	tx_head = tx_tail = tx_size = tx_transfer_size = 0;

	overflow = 0;
}

static volatile int switch_coding = 0;
static struct usb_cdc_line_coding line_coding;

int usb_serial_set_line_coding_cb(struct usb_cdc_line_coding *coding)
{
	(void)(coding);
	return 0;
}

int usb_serial_get_line_coding_cb(struct usb_cdc_line_coding *coding)
{
	(void)(coding);
	return 0;
}

static void switch_line_coding(void)
{
//	switch_coding = 0;
}

int usb_serial_set_control_line_state_cb(uint16_t state)
{
	(void)(state);
	return 0;
}

/* worst case, we've got tens of cycles to prevent a stutter */
void dma1_channel2_3_dma2_channel1_2_isr(void)
{
	DMA_CCR(UART_DMA, DMA_CHANNEL_TX) &=~ DMA_CCR_EN;
	DMA_IFCR(UART_DMA) = DMA_ISR_TCIF_TX;
	USART_ICR(UART) = USART_ICR_TCCF;

	uint32_t size = tx_size-tx_transfer_size;
	tx_size = size;

	uint32_t next = tx_head+tx_transfer_size;
	if (next >= UART_TX_RINGSIZE) /* really == */
		next = 0;

	if (size > UART_TX_RINGSIZE-next)
		size = UART_TX_RINGSIZE-next;

	if (size > PACKET_SIZE)
		size = PACKET_SIZE;

	DMA_CMAR(UART_DMA, DMA_CHANNEL_TX) = (uint32_t)&buf_uart_tx[next];
	DMA_CNDTR(UART_DMA, DMA_CHANNEL_TX) = size;

	if (size)
		DMA_CCR(UART_DMA, DMA_CHANNEL_TX) |= DMA_CCR_EN;

	tx_transfer_size = size;
	tx_head = next;
}

static void uart_tx_restart_dma(void)
{
	dma1_channel2_3_dma2_channel1_2_isr();
}

static void read_from_usb(void)
{
	cm_disable_interrupts();
	if ( !switch_coding && tx_size+PACKET_SIZE <= UART_TX_RINGSIZE )
	{
		cm_enable_interrupts();
		uint32_t n = usb_serial_read(&buf_uart_tx[tx_tail], PACKET_SIZE);
		cm_disable_interrupts();

		if (n != 0)
		{
			tx_size += n;
			tx_tail += n;
			if (tx_tail >= UART_TX_RINGSIZE)
			{
				tx_tail -= UART_TX_RINGSIZE;
				memcpy(&buf_uart_tx[0], &buf_uart_tx[UART_TX_RINGSIZE], tx_tail);
			}

			if (!( DMA_CCR(UART_DMA, DMA_CHANNEL_TX) & DMA_CCR_EN ) )
				uart_tx_restart_dma();
		}
	}
	cm_enable_interrupts();
}

static void write_to_usb(void)
{
	rx_head += usb_serial_write_noblock(&buf_uart_rx[rx_head], rx_transfer_size);
	if (rx_head >= UART_RX_RINGSIZE)
		rx_head -= UART_RX_RINGSIZE;
	rx_transfer_size = 0;
}

static void uart_update_buffer_state(void)
{
	uint32_t rx_prev = rx_tail;
	rx_tail = UART_RX_RINGSIZE-DMA_CNDTR(UART_DMA, DMA_CHANNEL_RX);

	if (rx_prev == rx_tail)
		return;

	if (rx_prev > rx_tail)
		rx_prev = 0;

	for (; rx_prev < PACKET_SIZE && rx_prev < rx_tail; rx_prev++)
		buf_uart_rx[UART_RX_RINGSIZE+rx_prev] = buf_uart_rx[rx_prev];

	uint32_t len = rx_tail-rx_head;
	if (len > UART_RX_RINGSIZE)
		len += UART_RX_RINGSIZE;

	if (len+OVERFLOW_DEADZONE > UART_RX_RINGSIZE)
	{
//		overflow = 1;
		rx_head = rx_tail + OVERFLOW_DEADZONE;
		if (rx_head >= UART_RX_RINGSIZE)
			rx_head -= UART_RX_RINGSIZE;
	}

	if (len > PACKET_SIZE)
		len = PACKET_SIZE;

	rx_transfer_size = len;
}

int main(void)
{
	init();
	for(;;)
	{
		usb_serial_poll();

		read_from_usb();

		uart_update_buffer_state();

		if (rx_transfer_size && usb_serial_can_write())
			write_to_usb();

		if ( switch_coding && tx_size == 0 )
			switch_line_coding();
	}

}

