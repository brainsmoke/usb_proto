#ifndef USB_SERIAL_H
#define USB_SERIAL_H

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

#define PACKET_SIZE_FULL_SPEED (64)

#include <stdint.h>
#include <stddef.h>
#include <libopencm3/usb/cdc.h>

/* usb serial (usb cdc acm) implementation which tries to minimize copying
 * data around.
 *
 * reads with buffer sizes >= PACKET_SIZE_FULL_SPEED will copy data directly
 * from the USB peripheral's packet memory (unless previously, a read was
 * issued with a smaller buffer size, leaving data in a small internal buffer.)
 *
 * Writes will be copied directly to packet memory.
 *
 * Optional callbacks may be implemented by the user.
 *
 */

void usb_serial_init(void);
void usb_serial_poll(void);
size_t usb_serial_read(uint8_t *buf, size_t len);
size_t usb_serial_write_noblock(const uint8_t *buf, size_t len);
size_t usb_serial_write(const uint8_t *buf, size_t len);

/* finish bulk transports with a zero-length packet if needed */
void usb_serial_flush(void);

int usb_serial_getchar(void);

int usb_serial_can_read(void);
int usb_serial_can_write(void);

/* 1 byte/packet */
void usb_serial_putchar(int c);

#define USB_SERIAL_STATE_RX_CARRIER    (1<<0)
#define USB_SERIAL_STATE_TX_CARRIER    (1<<1)
#define USB_SERIAL_STATE_BREAK         (1<<2)
#define USB_SERIAL_STATE_RING_SIGNAL   (1<<3)
#define USB_SERIAL_STATE_FRAMING_ERROR (1<<4)
#define USB_SERIAL_STATE_PARITY_ERROR  (1<<5)
#define USB_SERIAL_STATE_OVERRUN       (1<<6)

#define USB_SERIAL_STATE_DEFAULT (USB_SERIAL_STATE_RX_CARRIER|USB_SERIAL_STATE_TX_CARRIER)

void usb_serial_send_state(uint16_t serial_state);

/* --- CALLBACKS --- */
/* if implemented by user, will be called (from usb_serial_poll) when data is available */
void usb_serial_data_available_cb(void);

/* these should return 0 on failure and 1 on success */
int usb_serial_set_line_coding_cb(struct usb_cdc_line_coding *coding);
int usb_serial_get_line_coding_cb(struct usb_cdc_line_coding *coding);
#define USB_SERIAL_CONTROL_LINE_STATE_DTR (1<<0)
#define USB_SERIAL_CONTROL_LINE_STATE_RTS (1<<1)
#define USB_SERIAL_CONTROL_LINE_STATE_MASK (USB_SERIAL_CONTROL_LINE_STATE_DTR|USB_SERIAL_CONTROL_LINE_STATE_RTS)
int usb_serial_set_control_line_state_cb(uint16_t state);

#endif // USB_SERIAL_H
