
#include <libopencm3/usb/usbd.h>
#include <../libopencm3/lib/usb/usb_private.h>
#include <../libopencm3/lib/stm32/common/st_usbfs_core.h>
#include <libopencm3/stm32/st_usbfs.h>

#include "usb_double_buf.h"

#define USB_ENDPOINT_NORMAL_BITS     ( USB_EP_SETUP | USB_EP_TYPE | USB_EP_KIND | USB_EP_ADDR )
#define USB_ENDPOINT_TOGGLE_BITS     ( USB_EP_RX_STAT | USB_EP_TX_STAT | USB_EP_RX_DTOG | USB_EP_TX_DTOG )
#define USB_ENDPOINT_CLEAR_ONLY_BITS ( USB_EP_RX_CTR | USB_EP_TX_CTR )
#define USB_ENDPOINT_CLEAR_RX_CTR    ( USB_ENDPOINT_CLEAR_ONLY_BITS &~ USB_EP_RX_CTR )
#define USB_ENDPOINT_CLEAR_TX_CTR    ( USB_ENDPOINT_CLEAR_ONLY_BITS &~ USB_EP_TX_CTR )

#define USB_EP_RX_SWBUF USB_EP_TX_DTOG
#define USB_EP_TX_SWBUF USB_EP_RX_DTOG

#define USB_RX_ENDPOINT_SWAP_BUFFER(endpoint) \
	SET_REG( USB_EP_REG(endpoint), \
	       ( *USB_EP_REG(endpoint) & USB_ENDPOINT_NORMAL_BITS ) | USB_ENDPOINT_CLEAR_RX_CTR | USB_EP_RX_SWBUF )

#define USB_TX_ENDPOINT_SWAP_BUFFER(endpoint) \
	SET_REG( USB_EP_REG(endpoint), \
	       ( *USB_EP_REG(endpoint) & USB_ENDPOINT_NORMAL_BITS ) | USB_ENDPOINT_CLEAR_TX_CTR | USB_EP_TX_SWBUF )

#define USB_CLR_EP_RX_SWBUF(reg) USB_CLR_EP_TX_DTOG(reg)
#define USB_CLR_EP_TX_SWBUF(reg) USB_CLR_EP_RX_DTOG(reg)

#define USB_SET_EP_RX_SWBUF(endpoint) \
	SET_REG( USB_EP_REG(endpoint), \
	       ( ( *USB_EP_REG(endpoint) & (USB_ENDPOINT_NORMAL_BITS|USB_EP_RX_SWBUF) ) | USB_ENDPOINT_CLEAR_ONLY_BITS) ^ USB_EP_RX_SWBUF )

#define USB_SET_EP_TX_SWBUF(endpoint) \
	SET_REG( USB_EP_REG(endpoint), \
	       ( ( *USB_EP_REG(endpoint) & (USB_ENDPOINT_NORMAL_BITS|USB_EP_TX_SWBUF) ) | USB_ENDPOINT_CLEAR_ONLY_BITS) ^ USB_EP_TX_SWBUF )

typedef struct
{
	volatile uint16_t addr;
	volatile uint16_t count;

} usb_buffer_t;

typedef struct
{
	usb_buffer_t buf[2];

} usb_buffer_pair_t;

typedef struct
{
	usb_buffer_pair_t endpoints[8];

} usb_endpoint_buffers_t;

static usb_endpoint_buffers_t * const btable = (usb_endpoint_buffers_t *)USB_PMA_BASE; /* + USB_GET_BTABLE, which is 0 */

#define USB_BUFFER_ADDR(buf) \
	(uint8_t *)(USB_PMA_BASE + (buf)->addr)

#define DBUF_FULL 1
#define DBUF_EMPTY 0
#define DBUF_NEW 2

uint8_t dbuf_state[8];

int usb_double_buffer_can_read(uint8_t endpoint)
{
	endpoint = endpoint & 0x7; //0x7f;

	if ( dbuf_state[endpoint] != DBUF_EMPTY )
		return 1;

	if ( !(*USB_EP_REG(endpoint) & USB_EP_RX_CTR) )
		return 0;

	USB_RX_ENDPOINT_SWAP_BUFFER(endpoint);
	dbuf_state[endpoint] = DBUF_FULL;

	return 1;
}

uint16_t usb_double_buffer_read_packet(uint8_t endpoint, uint8_t *buf, uint16_t len)
{
	endpoint = endpoint & 0x7; //0x7f;

	if ( dbuf_state[endpoint] == DBUF_EMPTY )
	{
		if ( *USB_EP_REG(endpoint) & USB_EP_RX_CTR )
			USB_RX_ENDPOINT_SWAP_BUFFER(endpoint);
		else
			return 0;
	}

	usb_buffer_t *pack_buf = &btable->endpoints[endpoint].buf[ (*USB_EP_REG(endpoint) & USB_EP_RX_SWBUF) ? 1 : 0 ];
	uint16_t packet_size = MIN(len, pack_buf->count & 0x3ff);
	st_usbfs_copy_from_pm(buf, USB_BUFFER_ADDR(pack_buf), packet_size);

	if ( *USB_EP_REG(endpoint) & USB_EP_RX_CTR )
	{
		USB_RX_ENDPOINT_SWAP_BUFFER(endpoint);
		dbuf_state[endpoint] = DBUF_FULL;
	}
	else
		dbuf_state[endpoint] = DBUF_EMPTY;

	return packet_size;
}

int usb_double_buffer_can_write(uint8_t endpoint)
{
	endpoint = endpoint & 0x7; //0x7f;

	if ( dbuf_state[endpoint] != DBUF_FULL )
		return 1;

	if ( !(*USB_EP_REG(endpoint) & USB_EP_TX_CTR) )
		return 0;

	USB_TX_ENDPOINT_SWAP_BUFFER(endpoint);
	dbuf_state[endpoint] = DBUF_EMPTY;

	return 1;
}

uint16_t usb_double_buffer_write_packet(uint8_t endpoint, const uint8_t *buf, uint16_t len)
{
	endpoint = endpoint & 0x7; //0x7f;

	if ( dbuf_state[endpoint] == DBUF_FULL )
	{
		if ( *USB_EP_REG(endpoint) & USB_EP_TX_CTR )
			USB_TX_ENDPOINT_SWAP_BUFFER(endpoint);
		else
			return 0;
	}

	usb_buffer_t *pack_buf = &btable->endpoints[endpoint].buf[ (*USB_EP_REG(endpoint) & USB_EP_TX_SWBUF) ? 1 : 0 ];
	st_usbfs_copy_to_pm(USB_BUFFER_ADDR(pack_buf), buf, len);
	pack_buf->count = len;

	if ( (*USB_EP_REG(endpoint) & USB_EP_TX_CTR) || (dbuf_state[endpoint] == DBUF_NEW) )
	{
		USB_TX_ENDPOINT_SWAP_BUFFER(endpoint);
		dbuf_state[endpoint] = DBUF_EMPTY;
	}
	else
		dbuf_state[endpoint] = DBUF_FULL;

	return len;
}

static uint16_t receive_buf_size(uint16_t max_size)
{
	if (max_size <= 62)
		return (max_size + 1) &~ 1;
	else
		return (max_size + 31) &~ 31;
}

static uint16_t receive_buf_blockfield(uint16_t max_size)
{
	uint16_t real_size = receive_buf_size(max_size);
	if (real_size <= 62)
		return real_size << (10-1);
	else
		return 0x8000 | ( real_size << (10-5) );
}

void usb_double_buffer_endpoint_setup(usbd_device *device, uint8_t endpoint, uint16_t max_size, usbd_endpoint_callback cb)
{
	uint8_t real_endpoint = endpoint & 0x7; //0x7f;

//	for (uint32_t i=device->pm_top; i<0x400; i+=2)
//		*(volatile uint16_t *)(0x40006000+i) = i;

	if ( endpoint & 0x80 ) /* TX */
	{
		dbuf_state[real_endpoint] = DBUF_NEW;
		USB_CLR_EP_TX_DTOG(real_endpoint);
		USB_CLR_EP_TX_SWBUF(real_endpoint);
		USB_SET_EP_RX_STAT(real_endpoint, USB_EP_RX_STAT_DISABLED);
		USB_SET_EP_RX_ADDR(real_endpoint, device->pm_top);
	}
	else /* RX */
	{
		dbuf_state[real_endpoint] = DBUF_EMPTY;
		USB_SET_EP_RX_SWBUF(real_endpoint);
		USB_CLR_EP_RX_DTOG(real_endpoint);
		USB_SET_EP_TX_STAT(real_endpoint, USB_EP_TX_STAT_DISABLED);
		USB_SET_EP_TX_ADDR(real_endpoint, device->pm_top);
		USB_SET_EP_TX_COUNT(real_endpoint, receive_buf_blockfield(max_size));
		max_size = receive_buf_size(max_size);
	}
	device->pm_top += max_size;
	usbd_ep_setup(device, endpoint, USB_ENDPOINT_ATTR_BULK, max_size, cb);

	USB_SET_EP_KIND(real_endpoint);
	if ( endpoint & 0x80 ) /* TX */
		USB_SET_EP_TX_STAT(real_endpoint, USB_EP_TX_STAT_VALID);

//
//	if ( endpoint & 0x80 )
  //  SET_REG( USB_EP_REG(real_endpoint), \
    //       ( ( *USB_EP_REG(real_endpoint) & (USB_ENDPOINT_NORMAL_BITS|USB_EP_TX_STAT_VALID) ) | USB_EP_KIND /* | USB_ENDPOINT_CLEAR_ONLY_BITS */) ^ USB_EP_TX_STAT_VALID );
	//else
//    SET_REG( USB_EP_REG(real_endpoint), \
  //         ( ( *USB_EP_REG(real_endpoint) & (USB_ENDPOINT_NORMAL_BITS|USB_EP_RX_STAT_VALID) ) | USB_EP_KIND /* | USB_ENDPOINT_CLEAR_ONLY_BITS */) ^ USB_EP_RX_STAT_VALID );

}

