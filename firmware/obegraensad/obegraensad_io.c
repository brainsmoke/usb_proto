
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "obegraensad.h"
#include "util.h"
#include "usb_serial.h"

static void millis_timer_init(void)
{
	rcc_periph_clock_enable(RCC_TIM14);
	TIM_ARR(TIM14) = 0xffff;
	TIM_PSC(TIM14) = F_CPU/1000;
	TIM_CR1(TIM14) = TIM_CR1_CEN;
}

static uint16_t millis_u16(void)
{
	return TIM_CNT(TIM14);
}


static const char hex[] = "0123456789ABCDEF";

uint16_t t0 = 0, state=BUTTON_UP;

#define PRESSED_MESSAGE "button pressed\r\n"
#define RELEASED_MESSAGE "button released\r\n"

static void io_poll(void)
{
	uint16_t t = millis_u16();

	if ( ( (t-t0)&0xffff ) > DEBOUNCE_MS )
	{
		uint16_t button_read = GPIO_IDR(GPIO_IN) & BIT_BUTTON;
		if (button_read != state)
		{
			state = button_read;	
			if (state == BUTTON_DOWN)
				usb_serial_write((uint8_t *)PRESSED_MESSAGE, sizeof(PRESSED_MESSAGE)-1);
			else
				usb_serial_write((uint8_t *)RELEASED_MESSAGE, sizeof(RELEASED_MESSAGE)-1);
			t0 = t;
		}
	}

}

#define PACKET_SIZE (64UL)

static uint8_t packet[PACKET_SIZE];

static uint32_t packet_len = 0, packet_ix = 0;

int get_u8(void)
{
	while ( !(packet_ix < packet_len) )
	{
		usb_serial_poll();
		packet_len = usb_serial_read(packet, PACKET_SIZE);
		packet_ix = 0;
		io_poll();
	}
	return packet[packet_ix++];
}

int get_u16le(void)
{
	int c;
	if ( packet_ix+1 < packet_len )
	{
		c = packet[packet_ix] | packet[packet_ix+1]<<8;
		packet_ix += 2;
	}
	else
		c = ( get_u8() ) | ( get_u8() << 8 );

	return c;
}

void init_io(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	remap_usb_pins();
	usb_serial_init();

	GPIO_ODR(GPIO_OUT) = BIT_NOT_OUTPUT_ENABLE;

	uint32_t pinmask = BIT_DATA_0 | BIT_DATA_1 | BIT_DATA_2 | BIT_DATA_3 | BIT_CLK | BIT_LATCH | BIT_NOT_OUTPUT_ENABLE;

	gpio_mode_setup(GPIO_OUT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, pinmask);
	gpio_set_output_options(GPIO_OUT, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, pinmask);

	gpio_mode_setup(GPIO_IN, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, BIT_BUTTON);

	millis_timer_init();
}

