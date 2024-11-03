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

/* Libopencm3 does not do double buffering on stm32, so we'll need to setup the
 * peripheral ourself.
 *
 */

#include <libopencm3/usb/usbd.h>
#include <../libopencm3/lib/usb/usb_private.h>
#include <../libopencm3/lib/stm32/common/st_usbfs_core.h>
#include <libopencm3/stm32/st_usbfs.h>

#include "usb_double_buf.h"

/* optional callbacks */
void __attribute__((weak)) usb_double_buffer_data_available_cb(uint8_t endpoint)
{
	(void)endpoint;
}

void __attribute__((weak)) usb_double_buffer_can_write_cb(uint8_t endpoint)
{
	(void)endpoint;
}

#define USB_ENDPOINT_NORMAL_BITS     ( USB_EP_SETUP | USB_EP_TYPE | USB_EP_KIND | USB_EP_ADDR )
#define USB_ENDPOINT_TOGGLE_BITS     ( USB_EP_RX_STAT | USB_EP_TX_STAT | USB_EP_RX_DTOG | USB_EP_TX_DTOG )
#define USB_ENDPOINT_CLEAR_ONLY_BITS ( USB_EP_RX_CTR | USB_EP_TX_CTR )
#define USB_ENDPOINT_CLEAR_RX_CTR    ( USB_ENDPOINT_CLEAR_ONLY_BITS &~ USB_EP_RX_CTR )
#define USB_ENDPOINT_CLEAR_TX_CTR    ( USB_ENDPOINT_CLEAR_ONLY_BITS &~ USB_EP_TX_CTR )


#define USB_ENDPOINT_BUFSTATE_BITS   ( USB_EP_RX_DTOG | USB_EP_TX_DTOG )
#define USB_ENDPOINT_BUSY(endpoint)  ( { uint32_t status = *USB_EP_REG(endpoint) & USB_ENDPOINT_BUFSTATE_BITS; (status != 0 && status != USB_ENDPOINT_BUFSTATE_BITS); } )

#define USB_EP_RX_SWBUF USB_EP_TX_DTOG
#define USB_EP_TX_SWBUF USB_EP_RX_DTOG

#define USB_RX_ENDPOINT_SWAP_BUFFER(endpoint) \
	SET_REG( USB_EP_REG(endpoint), \
	       ( *USB_EP_REG(endpoint) & USB_ENDPOINT_NORMAL_BITS ) | USB_ENDPOINT_CLEAR_RX_CTR | USB_EP_RX_SWBUF )

#define USB_TX_ENDPOINT_SWAP_BUFFER(endpoint) \
	SET_REG( USB_EP_REG(endpoint), \
	       ( *USB_EP_REG(endpoint) & USB_ENDPOINT_NORMAL_BITS ) | USB_ENDPOINT_CLEAR_TX_CTR | USB_EP_TX_SWBUF )

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
	((uint8_t *)(USB_PMA_BASE + (buf)->addr))

#define DBUF_EMPTY 0
#define DBUF_FULL 1

uint8_t dbuf_state[8];

int usb_double_buffer_can_read(uint8_t endpoint)
{
	endpoint &= 0x7; //0x7f;

	if ( dbuf_state[endpoint] != DBUF_EMPTY )
		return 1;

	if ( USB_ENDPOINT_BUSY(endpoint) )
		return 0;

	USB_RX_ENDPOINT_SWAP_BUFFER(endpoint);
	dbuf_state[endpoint] = DBUF_FULL;

	return 1;
}

uint16_t usb_double_buffer_read_packet(uint8_t endpoint, uint8_t *buf, uint16_t len)
{
	endpoint &= 0x7; //0x7f;

	if ( dbuf_state[endpoint] == DBUF_EMPTY )
	{
		if ( !USB_ENDPOINT_BUSY(endpoint) )
			USB_RX_ENDPOINT_SWAP_BUFFER(endpoint);
		else
			return 0;
	}

	usb_buffer_t *pack_buf = &btable->endpoints[endpoint].buf[ (*USB_EP_REG(endpoint) & USB_EP_RX_SWBUF) ? 1 : 0 ];
	uint16_t packet_size = MIN(len, pack_buf->count & 0x3ff);
	st_usbfs_copy_from_pm(buf, USB_BUFFER_ADDR(pack_buf), packet_size);

	if ( !USB_ENDPOINT_BUSY(endpoint) )
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
	endpoint &= 0x7; //0x7f;

	if ( dbuf_state[endpoint] == DBUF_EMPTY )
		return 1;

	if ( USB_ENDPOINT_BUSY(endpoint) )
		return 0;

	USB_TX_ENDPOINT_SWAP_BUFFER(endpoint);
	dbuf_state[endpoint] = DBUF_EMPTY;

	return 1;
}

uint16_t usb_double_buffer_write_packet(uint8_t endpoint, const uint8_t *buf, uint16_t len)
{
	endpoint &= 0x7; //0x7f;

	if ( dbuf_state[endpoint] != DBUF_EMPTY )
	{
		if ( !USB_ENDPOINT_BUSY(endpoint) )
			USB_TX_ENDPOINT_SWAP_BUFFER(endpoint);
		else
			return 0;
	}

	usb_buffer_t *pack_buf = &btable->endpoints[endpoint].buf[ (*USB_EP_REG(endpoint) & USB_EP_TX_SWBUF) ? 1 : 0 ];
	st_usbfs_copy_to_pm(USB_BUFFER_ADDR(pack_buf), buf, len);
	pack_buf->count = len;

	if ( !USB_ENDPOINT_BUSY(endpoint) )
	{
		USB_TX_ENDPOINT_SWAP_BUFFER(endpoint);
		dbuf_state[endpoint] = DBUF_EMPTY;
	}
	else
		dbuf_state[endpoint] = DBUF_FULL;

	return len;
}

static void _usb_double_buffer_data_read(usbd_device *dev, uint8_t endpoint)
{
	(void)dev;
	uint8_t real_endpoint = endpoint & 0x7;

	if ( dbuf_state[real_endpoint] == DBUF_EMPTY )
	{
		USB_RX_ENDPOINT_SWAP_BUFFER(real_endpoint);
		dbuf_state[real_endpoint] = DBUF_FULL;
	}

	usb_double_buffer_data_available_cb(endpoint);
}

static void _usb_double_buffer_data_written(usbd_device *dev, uint8_t endpoint)
{
	(void)dev;
	uint8_t real_endpoint = endpoint & 0x7;

	if ( dbuf_state[real_endpoint] != DBUF_EMPTY )
	{
		USB_TX_ENDPOINT_SWAP_BUFFER(real_endpoint);
		dbuf_state[real_endpoint] = DBUF_EMPTY;
	}

	usb_double_buffer_can_write_cb(endpoint);
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
		return 0x8000 | ( (real_size-32) << (10-5) );
}

static enum usbd_request_return_codes _usb_double_buf_stall_cb(usbd_device *device,
                                                               struct usb_setup_data *req,
                                                               uint8_t **buf, uint16_t *len,
                                                               usbd_control_complete_callback *complete)
{
	(void)device;
	(void)buf;
	(void)len;
	(void)complete;
	if (req->wValue == USB_FEAT_ENDPOINT_HALT)
	{
		if (req->bRequest == USB_REQ_CLEAR_FEATURE)
		{
			/* implement double buffer stall */
			return USBD_REQ_NOTSUPP;
		}
		if (req->bRequest == USB_REQ_SET_FEATURE)
		{
			/* implement double buffer stall */
			return USBD_REQ_NOTSUPP;
		}
	}
	return USBD_REQ_NEXT_CALLBACK;
}


void usb_double_buffer_endpoint_setup(usbd_device *device, uint8_t endpoint, uint16_t max_size)
{
	uint8_t real_endpoint = endpoint & 0x7; //0x7f;

	volatile uint32_t *ep_reg = USB_EP_REG(real_endpoint);

	max_size = receive_buf_size(max_size);
	USB_SET_EP_TX_ADDR(real_endpoint, device->pm_top);
	device->pm_top += max_size;
	USB_SET_EP_RX_ADDR(real_endpoint, device->pm_top);
	device->pm_top += max_size;


	dbuf_state[real_endpoint] = DBUF_EMPTY;
	if ( endpoint & 0x80 ) /* TX */
	{
		USB_SET_EP_RX_COUNT(real_endpoint, 0);
		USB_SET_EP_TX_COUNT(real_endpoint, 0);

		uint16_t init_normal_bits = USB_EP_TYPE_BULK | USB_EP_KIND | (real_endpoint & 0xf);
		uint16_t init_toggle_bits = USB_EP_TX_STAT_VALID | USB_EP_RX_STAT_DISABLED | USB_EP_TX_DTOG | USB_EP_TX_SWBUF;
		uint16_t init_clear_only_bits = USB_ENDPOINT_CLEAR_ONLY_BITS;

		*ep_reg = ( (*ep_reg & init_toggle_bits) ^ init_toggle_bits ) | init_normal_bits | init_clear_only_bits;
		device->user_callback_ctr[real_endpoint][USB_TRANSACTION_IN] = _usb_double_buffer_data_written;
	}
	else /* RX */
	{
		uint16_t count_field = receive_buf_blockfield(max_size);
		USB_SET_EP_RX_COUNT(real_endpoint, count_field);
		USB_SET_EP_TX_COUNT(real_endpoint, count_field);

		uint16_t init_normal_bits = USB_EP_TYPE_BULK | USB_EP_KIND | (real_endpoint & 0xf);
		uint16_t init_toggle_bits = USB_EP_RX_STAT_VALID | USB_EP_TX_STAT_DISABLED |/* USB_EP_RX_DTOG |*/ USB_EP_RX_SWBUF;
		uint16_t init_clear_only_bits = USB_ENDPOINT_CLEAR_ONLY_BITS;

		*ep_reg = ( (*ep_reg & init_toggle_bits) ^ init_toggle_bits ) | init_normal_bits | init_clear_only_bits;
		device->user_callback_ctr[real_endpoint][USB_TRANSACTION_OUT] = _usb_double_buffer_data_read;
	}

	static int once = 0;
	if (!once)
	{
		usbd_register_control_callback(device, USB_REQ_TYPE_ENDPOINT, USB_REQ_TYPE_RECIPIENT, _usb_double_buf_stall_cb);
		once = 1;
	}
}

