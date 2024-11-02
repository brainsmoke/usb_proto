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

static void uart_enable(void)
{
	USART_CR1(UART) |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

static void uart_disable(void)
{
	USART_CR1(UART) &=~ (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE);
}

static void uart_set_parity(uint32_t parity)
{
	USART_CR1(UART) &=~ USART_PARITY_MASK;
	USART_CR1(UART) |= parity;
}

static void uart_set_stop_bits(uint32_t stop_bits)
{
	USART_CR2(UART) &=~ USART_CR2_STOPBITS_MASK;
	USART_CR2(UART) |= stop_bits;
}

static void uart_set_word_length(uint32_t length)
{
	USART_CR1(UART) &=~ (USART_CR1_M0|USART_CR1_M1);
	switch (length)
	{
		case 7:
			USART_CR1(UART) |= USART_CR1_M1;
			break;
		case 9:
			break;
			USART_CR1(UART) |= USART_CR1_M0;
		case 8:
		default:
			break;
	}
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
	uint32_t ospeed = GPIO_OSPEED_LOW;
	if (baudrate > MAX_LOWSPEED_BAUDRATE)
		ospeed = GPIO_OSPEED_MED;

	gpio_set_output_options(UART_TX_PORT, GPIO_OTYPE_PP, ospeed, UART_TX_PIN);
}

static void uart_init(void)
{
	uart_set_baudrate(DEFAULT_BAUDRATE);
	uart_rx_init();
	uart_tx_init();
	DMA_CCR(UART_DMA, DMA_CHANNEL_RX) |= DMA_CCR_EN;
	uart_enable();
}

void wwdg_isr(void)
{
	reset_system();
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

	remap_usb_pins();
	usb_serial_init();
	uart_init();

	gpio_write_mask(PORT_LINE_STATE, LINE_STATE_DEFAULT, LINE_STATE_PIN_MASK);
	gpio_mode_setup(PORT_LINE_STATE, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LINE_STATE_PIN_MASK);

	rx_head = rx_tail = rx_transfer_size = 0;
	tx_head = tx_tail = tx_size = tx_transfer_size = 0;

	overflow = 0;
}

static volatile int switch_coding = 0;
static struct usb_cdc_line_coding line_coding;

static const uint32_t stop_bits_map[] =
{
	[USB_CDC_1_STOP_BITS] = USART_CR2_STOPBITS_1,
	[USB_CDC_1_5_STOP_BITS] = USART_CR2_STOPBITS_1_5,
	[USB_CDC_2_STOP_BITS] = USART_CR2_STOPBITS_2,
};

static const uint32_t parity_map[] =
{
	[USB_CDC_NO_PARITY] = 0,
	[USB_CDC_ODD_PARITY] = USART_PARITY_ODD,
	[USB_CDC_EVEN_PARITY] = USART_PARITY_EVEN,
};

static int line_coding_ok(struct usb_cdc_line_coding *coding)
{
	return (coding->bCharFormat <= USB_CDC_2_STOP_BITS) &&
	       (coding->bParityType <= USB_CDC_EVEN_PARITY) && /* mark/space parity not supported */
	       (coding->dwDTERate <= MAX_BAUDRATE) &&
	       (coding->dwDTERate >= MIN_BAUDRATE) &&
	     ( (coding->bDataBits == 7) || (coding->bDataBits == 8) );
}


int usb_serial_set_line_coding_cb(struct usb_cdc_line_coding *coding)
{
	if ( !line_coding_ok(coding) )
		return 0;

	if ( memcmp(&line_coding, coding, sizeof(line_coding)) != 0 )
	{
		line_coding = *coding;
		switch_coding = 1;
	}

	return 1;
}

int usb_serial_get_line_coding_cb(struct usb_cdc_line_coding *coding)
{
	*coding = line_coding;
	return 1;
}

static void switch_line_coding(void)
{
	uart_disable();
	uart_set_baudrate(line_coding.dwDTERate);
	uart_set_parity(parity_map[line_coding.bParityType]);
	uart_set_stop_bits(stop_bits_map[line_coding.bCharFormat]);
	uart_set_word_length(line_coding.bDataBits);
	uart_enable();
	switch_coding = 0;
}

/* DTR/RTS are active low, PIN_ESP_GPIO0 and PIN_ESP_EN should match the reset circuitry esptool.py expects */
static const uint32_t control_state_map[] =
{
	[0] =                                                                    PIN_RTS | PIN_DTR | PIN_ESP_GPIO0 | PIN_ESP_EN ,
	[USB_SERIAL_CONTROL_LINE_STATE_DTR] =                                    PIN_RTS |           PIN_ESP_GPIO0              ,
	[USB_SERIAL_CONTROL_LINE_STATE_RTS] =                                              PIN_DTR |                 PIN_ESP_EN ,
	[USB_SERIAL_CONTROL_LINE_STATE_RTS|USB_SERIAL_CONTROL_LINE_STATE_DTR] =                      PIN_ESP_GPIO0 | PIN_ESP_EN ,
};

int usb_serial_set_control_line_state_cb(uint16_t state)
{
	uint32_t pin_state = control_state_map[state&USB_SERIAL_CONTROL_LINE_STATE_MASK];
//	gpio_write_mask(PORT_LINE_STATE, pin_state, LINE_STATE_PIN_MASK);
	usb_serial_send_state(USB_SERIAL_STATE_DEFAULT);
	return 1;
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
	overflow = 0;
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
		if (!overflow)
			usb_serial_send_state(USB_SERIAL_STATE_DEFAULT | USB_SERIAL_STATE_OVERRUN);
		overflow = 1;
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

	usb_serial_send_state(USB_SERIAL_STATE_DEFAULT);

	for(;;)
	{
		usb_serial_poll();

		read_from_usb();

		uart_update_buffer_state();

		if (rx_transfer_size)
			write_to_usb();
		else
			usb_serial_flush();

		if ( switch_coding && tx_size == 0 )
			switch_line_coding();
	}

}

