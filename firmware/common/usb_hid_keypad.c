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
#include <libopencm3/usb/hid.h>

#include <string.h>

#include "config.h"
#include "usb_hid_keypad.h"

#ifndef ID_VENDOR
#define ID_VENDOR  (0x4242)
#endif
#ifndef ID_PRODUCT
#define ID_PRODUCT (0x4242)
#endif
#ifndef ID_VERSION
#define ID_VERSION (0x0000)
#endif
#ifndef MANUFACTURER_STRING
#define MANUFACTURER_STRING "manufacturer"
#endif
#ifndef PRODUCT_STRING
#define PRODUCT_STRING "product"
#endif
#ifndef SERIAL_STRING
#define SERIAL_STRING "00000001"
#endif

static usbd_device *device;

static uint8_t report_descriptor[256];
static uint8_t report[32];
static uint8_t control[128];

static uint32_t keymap[HID_KEYPAD_MAX_KEYS];

static uint32_t report_bits=0;
static volatile uint32_t need_update, usb_ready;

#define VERSION_USB_2_0 (0x0200)

#define DEVICE_CLASS_LOOK_AT_INTERFACE (0)
#define NO_SUBCLASS (0)
#define NO_PROTOCOL (0)
#define PACKET_SIZE_FULL_SPEED (64)

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
	[MANUFACTURER-1] = MANUFACTURER_STRING,
	[PRODUCT-1]      = PRODUCT_STRING,
	[SERIAL-1]       = SERIAL_STRING,
};
#define N_STRING_DESCRIPTORS (sizeof(string_descriptors)/sizeof(string_descriptors[0]))

/* non-static descriptors */

/* .wMaxPacketSize needs to be set */
static struct usb_endpoint_descriptor endpoint_desc =
{
	.bLength           = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType   = USB_DT_ENDPOINT,
	.bEndpointAddress  = USB_ENDPOINT_ADDR_IN(1),
	.bmAttributes      = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize    = 0x4242,
	.bInterval         = MILLISECONDS(1),
};

typedef struct __attribute__((packed))
{
	struct usb_hid_descriptor d;
	uint8_t bDescriptorType;
	uint16_t wDescriptorLength;

} single_report_hid_descriptor_t;

/* .wDescriptorLength to be set */
static single_report_hid_descriptor_t usb_hid_desciptor =
{
	.d.bLength = sizeof(single_report_hid_descriptor_t),
	.d.bDescriptorType = USB_HID_DT_HID,
	.d.bcdHID = 0x111, /* 1.11 */
	.d.bCountryCode = COUNTRY_NONE,
	.d.bNumDescriptors = 1,
	.bDescriptorType = USB_HID_DT_REPORT,
	.wDescriptorLength = 0x4242,
};

static size_t report_size(size_t n_keys)
{
	return (n_keys+7)>>3;
}

static size_t create_hid_keypad_descriptor(uint8_t buf[], size_t len, const uint32_t keys[], size_t n_keys)
{
	const uint8_t hid_keypad_prologue[] =
	{
        0x05,0x01,      /* Usage Page (Generic Desktop) */
        0x09,0x07,      /* Usage (Keypad) */
        0xa1,0x01,      /* Collection (Application) */
	};

	if (n_keys >= 256)
		return 0;

	size_t descriptor_size = sizeof(hid_keypad_prologue) + n_keys*5 + 7;
	if (n_keys & 0x7)
		descriptor_size += 6;

	if (len < descriptor_size) return 0;

	memcpy(&buf[0], hid_keypad_prologue, sizeof(hid_keypad_prologue));

	size_t off = sizeof(hid_keypad_prologue);
	size_t i;

	for (i=0; i<n_keys; i++)
	{
		buf[off++] = 0x0b;                 // (Long) Usage
		buf[off++] = (keys[i]>>0)  & 0xff; // key
		buf[off++] = (keys[i]>>8)  & 0xff;
		buf[off++] = (keys[i]>>16) & 0xff; // page
		buf[off++] = (keys[i]>>24) & 0xff;
	}

	buf[off++] = 0x75;   //  Report Size (1)
	buf[off++] = 0x01;

	buf[off++] = 0x95;   //  Report Count (n_keys) */
	buf[off++] = n_keys;

	buf[off++] = 0x81;   //  Input (Data, Variable, Absolute)
	buf[off++] = 0x02;

	if (n_keys & 0x7)
	{
		buf[off++] = 0x75;   //  Report Size (8- (n_keys&0x7) )
		buf[off++] = (0x8-(n_keys&0x7) );

		buf[off++] = 0x95;   //  Report Count (1) */
		buf[off++] = 1;

		buf[off++] = 0x81;   //  Input (Constant)
		buf[off++] = 0x01;
	}

	buf[off++] = 0xC0;   //  End Collection

	return off;
}


static const struct usb_device_descriptor device_desc =
{
	.bLength            = USB_DT_DEVICE_SIZE,
	.bDescriptorType    = USB_DT_DEVICE,
	.bcdUSB             = VERSION_USB_2_0,
	.bDeviceClass       = DEVICE_CLASS_LOOK_AT_INTERFACE,
	.bDeviceSubClass    = NO_SUBCLASS,
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

static const struct usb_interface_descriptor interface_desc =
{
	.bLength            = USB_DT_INTERFACE_SIZE,
	.bDescriptorType    = USB_DT_INTERFACE,
	.bInterfaceNumber   = 0,
	.bAlternateSetting  = 0,
	.bNumEndpoints      = 1,
	.bInterfaceClass    = USB_CLASS_HID,
	.bInterfaceSubClass = USB_HID_SUBCLASS_NO,
	.bInterfaceProtocol = USB_HID_INTERFACE_PROTOCOL_KEYBOARD,
	.iInterface         = NO_STRING,

	.endpoint           = &endpoint_desc,
	.extra              = &usb_hid_desciptor,
	.extralen           = sizeof(usb_hid_desciptor),
};

const struct usb_interface interfaces[] =
{
	{
		.num_altsetting = 1,
		.altsetting = &interface_desc,
	},
};

static const struct usb_config_descriptor config_desc =
{
	.bLength             = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType     = USB_DT_CONFIGURATION,
	.wTotalLength        = 0, /* calculated by USB stack */
	.bNumInterfaces      = 1,
	.bConfigurationValue = 1,
	.iConfiguration      = NO_STRING,
	.bmAttributes        = BUS_POWERED,
	.bMaxPower           = MILLIAMPS(250),

	.interface = interfaces,
};


static enum usbd_request_return_codes hid_control_callback(usbd_device *dev,
                                                           struct usb_setup_data *req,
                                                           uint8_t **buf, uint16_t *len,
                                                           usbd_control_complete_callback *complete)
{
	(void)dev;
	(void)complete;

	if( req->bmRequestType != REQ_INTERFACE ||
        req->bRequest != USB_REQ_GET_DESCRIPTOR ||
	    req->wValue != (USB_HID_DT_REPORT<<8) )
		return USBD_REQ_NOTSUPP;

	*buf = report_descriptor;
	*len = usb_hid_desciptor.wDescriptorLength;

	usb_ready=1;

	return USBD_REQ_HANDLED;
}

static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;

	usbd_ep_setup(dev, USB_ENDPOINT_ADDR_IN(1), USB_ENDPOINT_ATTR_INTERRUPT, endpoint_desc.wMaxPacketSize, NULL);

	usbd_register_control_callback(dev,
	                               USB_REQ_TYPE_STANDARD|USB_REQ_TYPE_INTERFACE,
	                               USB_REQ_TYPE_TYPE|USB_REQ_TYPE_RECIPIENT,
	                               hid_control_callback);
}

int usb_hid_keypad_init(const uint32_t keys[], size_t n_keys)
{
	if (n_keys > HID_KEYPAD_MAX_KEYS)
		return 0;

	size_t i, j;
	for (i=0, j=0; i<n_keys; i++)
		if (keys[i] != ~0UL)
			keymap[j++] = i;
	n_keys = j;


	memcpy(keymap, keys, sizeof(uint32_t)*n_keys);

	size_t len = create_hid_keypad_descriptor(report_descriptor, sizeof(report_descriptor),
		                                      keys, n_keys);

	if (len == 0)
		return 0;

	usb_hid_desciptor.wDescriptorLength = len;

	endpoint_desc.wMaxPacketSize = report_size(n_keys);
	report_bits = n_keys;

	device = usbd_init(&st_usbfs_v2_usb_driver, &device_desc, &config_desc,
	                   string_descriptors, N_STRING_DESCRIPTORS,
	                   control, sizeof(control));

	usbd_register_set_config_callback(device, hid_set_config);

	need_update = 1;
	usb_ready = 0;

	return 1;
}

static int get_index(uint32_t hid_key)
{
	size_t i;
	for (i=0; i<report_bits; i++)
		if (keymap[i] == hid_key)
			return i;

	return -1;
}

void usb_hid_keypad_key_up(uint32_t hid_key)
{
	int ix = get_index(hid_key);
	if (ix < 0)
		return;

	report[ix>>3] &=~ (1<<(ix&0x7));
	need_update = 1;
}

void usb_hid_keypad_key_down(uint32_t hid_key)
{
	int ix = get_index(hid_key);
	if (ix < 0)
		return;

	report[ix>>3] |= 1<<(ix&0x7);
	need_update = 1;
}

void usb_hid_keypad_poll(void)
{
	if (usb_ready && need_update)
	{
		need_update = 0;
		if (usbd_ep_write_packet(device, endpoint_desc.bEndpointAddress,
		                         report, endpoint_desc.wMaxPacketSize) !=
		                                 endpoint_desc.wMaxPacketSize)
			need_update = 1;
	}
	usbd_poll(device);
}

