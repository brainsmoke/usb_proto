/*
 * Copyright (c) 2023 Erik Bosman <erik@minemu.org>
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

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include <string.h>

#include "config.h"
#include "usb_serial.h"

#ifndef ID_VENDOR
#define ID_VENDOR  (0x4242)
#endif
#ifndef ID_PRODUCT
#define ID_PRODUCT (0x4343)
#endif
#ifndef ID_VERSION
#define ID_VERSION (0x0000)
#endif

static usbd_device *device;

static uint8_t control[128];

static volatile uint32_t usb_ready;

#define VERSION_USB_2_0 (0x0200)

#define DEVICE_CLASS_LOOK_AT_INTERFACE (0)
#define NO_SUBCLASS (0)
#define NO_PROTOCOL (0)
#define PACKET_SIZE_FULL_SPEED (64)
#define CONTROL_PACKET_SIZE (16)

#define BUS_POWERED   (1<<7)
#define SELF_POWERED  (1<<6)
#define REMOTE_WAKEUP (1<<5)

#define MILLIAMPS(x)    ((x+1)>>1)
#define MILLISECONDS(x)  (x)

#define COUNTRY_NONE (0)

#define REQ_DEVICE_TO_HOST (0x80)
#define REQ_INTERFACE      (REQ_DEVICE_TO_HOST|0x01)

enum
{
	NO_STRING = 0,
    MANUFACTURER,
    PRODUCT,
    SERIAL,
};

static const char *const string_descriptors[] =
{
    [MANUFACTURER-1] = "techinc",
    [PRODUCT-1]      = "serial keypad",
    [SERIAL-1]       = "00000001",
};
#define N_STRING_DESCRIPTORS (sizeof(string_descriptors)/sizeof(string_descriptors[0]))

/* common descriptor for all USB devices */
static const struct usb_device_descriptor device_desc =
{
	.bLength            = USB_DT_DEVICE_SIZE,
	.bDescriptorType    = USB_DT_DEVICE,
	.bcdUSB             = VERSION_USB_2_0,
	.bDeviceClass       = USB_CLASS_CDC,
	.bDeviceSubClass    = USB_CDC_SUBCLASS_ACM,
	.bDeviceProtocol    = NO_PROTOCOL,
	.bMaxPacketSize0    = PACKET_SIZE_FULL_SPEED,
	.bNumConfigurations = 1,

	.idVendor           = ID_VENDOR,
	.idProduct          = ID_PRODUCT,

	.bcdDevice          = ID_VERSION,

	.iManufacturer      = MANUFACTURER,
	.iProduct           = PRODUCT,
	.iSerialNumber      = SERIAL,
};

const struct usb_interface interfaces[];

static const struct usb_config_descriptor config_desc =
{
	.bLength             = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType     = USB_DT_CONFIGURATION,
	.wTotalLength        = 0, /* calculated by USB stack */
	.bNumInterfaces      = 2,
	.bConfigurationValue = 1,
	.iConfiguration      = NO_STRING,
	.bmAttributes        = BUS_POWERED,
	.bMaxPower           = MILLIAMPS(250),

	.interface = interfaces,
};

#define UART_HOST_TO_DEVICE_ENDPOINT (1)
#define UART_DEVICE_TO_HOST_ENDPOINT (1)
#define UART_NOTIFICATION_ENDPOINT   (2)

#define UART_NOTIFICATION_INTERFACE  (0)
#define UART_DATA_INTERFACE          (1)

static struct usb_endpoint_descriptor data_endpoints[] =
{
	/* endpoints for serial-data interface */

	/* usb serial host -> device port */
	{
		.bLength           = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType   = USB_DT_ENDPOINT,
		.bEndpointAddress  = USB_ENDPOINT_ADDR_OUT(UART_HOST_TO_DEVICE_ENDPOINT),
		.bmAttributes      = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize    = PACKET_SIZE_FULL_SPEED,
		.bInterval         = MILLISECONDS(1),
	},
	/* usb serial device -> host port */
	{
		.bLength           = USB_DT_ENDPOINT_SIZE,
		.bDescriptorType   = USB_DT_ENDPOINT,
		.bEndpointAddress  = USB_ENDPOINT_ADDR_IN(UART_DEVICE_TO_HOST_ENDPOINT),
		.bmAttributes      = USB_ENDPOINT_ATTR_BULK,
		.wMaxPacketSize    = PACKET_SIZE_FULL_SPEED,
		.bInterval         = MILLISECONDS(1),
	},
};
#define DATA_OUT_ENDPOINT (&data_endpoints[0])
#define DATA_IN_ENDPOINT (&data_endpoints[1])

static struct usb_endpoint_descriptor notification_endpoint =
/* endpoints for serial notification interface */

/* usb serial notification channel port */
{
	.bLength           = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType   = USB_DT_ENDPOINT,
	.bEndpointAddress  = USB_ENDPOINT_ADDR_IN(UART_NOTIFICATION_ENDPOINT),
	.bmAttributes      = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize    = CONTROL_PACKET_SIZE,
	.bInterval         = MILLISECONDS(255),
};


static const struct __attribute__((packed))
{
	struct usb_cdc_header_descriptor          header_desc;
	struct usb_cdc_union_descriptor           union_desc;
	struct usb_cdc_call_management_descriptor call_desc;
	struct usb_cdc_acm_descriptor             acm_desc;

} functional_desc =
{
	.header_desc =
	{
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,
		.bcdCDC = 0x0110,
	},
	.union_desc =
	{
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,
		.bControlInterface = UART_NOTIFICATION_INTERFACE,
		.bSubordinateInterface0 = UART_DATA_INTERFACE,
	},
	.call_desc =
	{
		.bFunctionLength = sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
		.bmCapabilities = 0,
		.bDataInterface = UART_DATA_INTERFACE,
	},
	.acm_desc =
	{
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,
		.bmCapabilities = 0,
	},
};


static const struct usb_interface_descriptor notif_interface_desc =
{
	.bLength            = USB_DT_INTERFACE_SIZE,
	.bDescriptorType    = USB_DT_INTERFACE,
	.bInterfaceNumber   = UART_NOTIFICATION_INTERFACE,
	.bAlternateSetting  = 0,
	.bNumEndpoints      = 1,
	.bInterfaceClass    = USB_CLASS_CDC,
	.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
	.bInterfaceProtocol = USB_CDC_PROTOCOL_NONE,
	.iInterface         = NO_STRING,

	.endpoint           = &notification_endpoint,

	.extra              = &functional_desc,
	.extralen           = sizeof(functional_desc),
};

static const struct usb_interface_descriptor data_interface_desc =
{
	.bLength            = USB_DT_INTERFACE_SIZE,
	.bDescriptorType    = USB_DT_INTERFACE,
	.bInterfaceNumber   = UART_DATA_INTERFACE,
	.bAlternateSetting  = 0,
	.bNumEndpoints      = 2,
	.bInterfaceClass    = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = USB_CDC_PROTOCOL_NONE,
	.iInterface         = NO_STRING,

	.endpoint           = data_endpoints,
};

const struct usb_interface interfaces[] =
{
	{
		.num_altsetting = 1,
		.altsetting = &notif_interface_desc,
	},
	{
		.num_altsetting = 1,
		.altsetting = &data_interface_desc,
	},
};

static void endpoint_setup(usbd_device *dev,
                           const struct usb_endpoint_descriptor *desc,
                           usbd_endpoint_callback callback)
{
	usbd_ep_setup(dev, desc->bEndpointAddress, desc->bmAttributes, desc->wMaxPacketSize, callback);
}

/* just read the packet 'beyond' the buffer, copy the wraparound */
static uint8_t rx_buf[USB_RX_BUFSIZE+PACKET_SIZE_FULL_SPEED];

static size_t rx_start, rx_len;

/* not re-entrant-safe, assumes polling */
static void serial_rx_cb(usbd_device *dev, uint8_t ep)
{
	size_t rx_end = rx_start+rx_len;
	if (rx_end > USB_RX_BUFSIZE)
		rx_end -= USB_RX_BUFSIZE;

	size_t len = PACKET_SIZE_FULL_SPEED;
	if (len > USB_RX_BUFSIZE - rx_len)
		len = USB_RX_BUFSIZE - rx_len;

	len = usbd_ep_read_packet(dev, ep, &rx_buf[rx_end], len);
	rx_len += len;
	rx_end += len;

	if (rx_end > USB_RX_BUFSIZE)
		memcpy(&rx_buf[0], &rx_buf[USB_RX_BUFSIZE], rx_end - USB_RX_BUFSIZE);
}

size_t usb_serial_read(uint8_t *buf, size_t len)
{
	if (len > rx_len)
		len = rx_len;

	rx_len -= len;

	if (len >= USB_RX_BUFSIZE-rx_start)
	{
		memcpy(buf, &rx_buf[rx_start], USB_RX_BUFSIZE-rx_start);
		rx_start = len - (USB_RX_BUFSIZE-rx_start);
		memcpy(buf, &rx_buf[0], rx_start);
	}
	else
	{
		memcpy(buf, &rx_buf[rx_start], len);
		rx_start += len;
	}

	return len;
}

int usb_serial_getchar(void)
{
	if (rx_len == 0)
		return -1;

	int c = rx_buf[rx_start++];

	if (rx_start >= USB_RX_BUFSIZE)
		rx_start = 0;

	rx_len -= 1;

	return c;
}

void usb_serial_putchar(int c)
{
	uint8_t buf = c;
	while ( usbd_ep_write_packet(device, UART_DEVICE_TO_HOST_ENDPOINT, &buf, 1) == 0 )
	{
		usbd_poll(device);
	}
}

size_t usb_serial_write_noblock(const uint8_t *buf, size_t len)
{
	if (!usb_ready)
		return 0;

	if (len > PACKET_SIZE_FULL_SPEED)
		len = PACKET_SIZE_FULL_SPEED;

	return usbd_ep_write_packet(device, UART_DEVICE_TO_HOST_ENDPOINT, buf, len);
}

void usb_serial_poll(void)
{
	usbd_poll(device);
}

static enum usbd_request_return_codes serial_control_callback(usbd_device *dev,
                                                              struct usb_setup_data *req,
                                                              uint8_t **buf, uint16_t *len,
                                                              usbd_control_complete_callback *complete)
{
	(void)dev;
	(void)complete;
	(void)buf;
	(void)req;
	(void)len;

	return USBD_REQ_NOTSUPP;
}

static void serial_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;

	endpoint_setup(dev, DATA_OUT_ENDPOINT, serial_rx_cb);
	endpoint_setup(dev, DATA_IN_ENDPOINT, NULL);
	endpoint_setup(dev, &notification_endpoint, NULL);

	usbd_register_control_callback(dev,
	                               USB_REQ_TYPE_CLASS|USB_REQ_TYPE_INTERFACE,
	                               USB_REQ_TYPE_TYPE|USB_REQ_TYPE_RECIPIENT,
	                               serial_control_callback);
	usb_ready = 1;
}

void usb_serial_init(void)
{
	memset(rx_buf, 0, sizeof(rx_buf));
	rx_len = 0;
	rx_start = 0;

	device = usbd_init(&st_usbfs_v2_usb_driver, &device_desc, &config_desc,
	                   string_descriptors, N_STRING_DESCRIPTORS,
	                   control, sizeof(control));

	usbd_register_set_config_callback(device, serial_set_config);

	usb_ready = 0;
}

