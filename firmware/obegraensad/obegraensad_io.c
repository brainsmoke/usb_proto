
#include <libopencm3/stm32/rcc.h>

#include "obegraensad.h"
#include "util.h"
#include "usb_serial.h"

#define PACKET_SIZE (64UL)

static uint8_t packet[PACKET_SIZE];

static int packet_len = 0, packet_ix = 0;

int get_u8(void)
{
	while ( !(packet_ix < packet_len) )
	{
		packet_len = usb_serial_read(packet, PACKET_SIZE);
		packet_ix = 0;
		usb_serial_poll();
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
}

