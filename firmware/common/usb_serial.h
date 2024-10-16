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

#ifndef MANUFACTURER_STRING
#define MANUFACTURER_STRING "manufacturer"
#endif


#ifndef PRODUCT_STRING
#define PRODUCT_STRING "product"
#endif

#ifndef SERIAL_STRING
#define SERIAL_STRING "00000001"
#endif

void usb_serial_init(void);
void usb_serial_poll(void);
size_t usb_serial_read(uint8_t *buf, size_t len);
size_t usb_serial_write_noblock(const uint8_t *buf, size_t len);

int usb_serial_getchar(void);

/* 1 byte/packet */
void usb_serial_putchar(int c);


/* if implemented by user, will be called (from usb_serial_poll) when data is available */
void usb_data_available_cb(void);

#endif // USB_SERIAL_H
