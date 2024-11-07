#ifndef HID_KEYDEF_H
#define HID_KEYDEF_H

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

#define HID_KEY(page, key) ( (((page)&0xffff)<<16) | ((key)&0xffff) )
#define HID_GENERIC_DESKTOP_PAGE (0x01)
#define HID_KEYBOARD_PAGE (0x07)
#define HID_CONSUMER_PAGE (0x0c)

#define KEY_NONE (~0UL)

/* key definitions from: https://www.usb.org/sites/default/files/hut1_4.pdf */
#define KEY_RETURN              (HID_KEY(HID_KEYBOARD_PAGE, 0x28))
#define KEY_ESCAPE              (HID_KEY(HID_KEYBOARD_PAGE, 0x29))
#define KEY_BACKSPACE           (HID_KEY(HID_KEYBOARD_PAGE, 0x2A))
#define KEY_TAB                 (HID_KEY(HID_KEYBOARD_PAGE, 0x2B))
#define KEY_SPACEBAR            (HID_KEY(HID_KEYBOARD_PAGE, 0x2C))
#define KEY_NUMBER(n)           (HID_KEY(HID_KEYBOARD_PAGE, 0x1E + (n + 1) % 10))
#define KEY_ALPHA(n)            (HID_KEY(HID_KEYBOARD_PAGE, 0x4 + (n)))

#define KEY_LEFT_CONTROL        (HID_KEY(HID_KEYBOARD_PAGE, 0xE0)
#define KEY_LEFT_SHIFT          (HID_KEY(HID_KEYBOARD_PAGE, 0xE1)
#define KEY_LEFT_ALT            (HID_KEY(HID_KEYBOARD_PAGE, 0xE2)
#define KEY_LEFT_WINDOWS        (HID_KEY(HID_KEYBOARD_PAGE, 0xE3)
#define KEY_RIGHT_CONTROL       (HID_KEY(HID_KEYBOARD_PAGE, 0xE4)
#define KEY_RIGHT_SHIFT         (HID_KEY(HID_KEYBOARD_PAGE, 0xE5)
#define KEY_RIGHT_ALT           (HID_KEY(HID_KEYBOARD_PAGE, 0xE6)
#define KEY_RIGHT_WINDOWS       (HID_KEY(HID_KEYBOARD_PAGE, 0xE7)

#define KEY_PRINTSCREEN         (HID_KEY(HID_KEYBOARD_PAGE, 0x46)
#define KEY_SCROLL_LOCK         (HID_KEY(HID_KEYBOARD_PAGE, 0x47)

#define KEY_INSERT              (HID_KEY(HID_KEYBOARD_PAGE, 0x49)
#define KEY_HOME                (HID_KEY(HID_KEYBOARD_PAGE, 0x4A)
#define KEY_PAGE_UP             (HID_KEY(HID_KEYBOARD_PAGE, 0x4B)
#define KEY_DELETE              (HID_KEY(HID_KEYBOARD_PAGE, 0x4C)
#define KEY_END                 (HID_KEY(HID_KEYBOARD_PAGE, 0x4D)
#define KEY_PAGE_DOWN           (HID_KEY(HID_KEYBOARD_PAGE, 0x4E)
#define KEY_RIGHT_ARROW         (HID_KEY(HID_KEYBOARD_PAGE, 0x4F)
#define KEY_LEFT_ARROW          (HID_KEY(HID_KEYBOARD_PAGE, 0x50)
#define KEY_DOWN_ARROW          (HID_KEY(HID_KEYBOARD_PAGE, 0x51)

#define KEY_CAPSLOCK            (HID_KEY(HID_KEYBOARD_PAGE, 0x39)

/*  don't invoke with side-effect in argmuent */
#define KEY_FUNCTION(n)         (HID_KEY(HID_KEYBOARD_PAGE, \
	n <   1 ? KEY_NONE    : \
	n <= 12 ? 0x3A+(n-1)  : \
	n <= 24 ? 0x68+(n-13) : \
	KEY_NONE \
)

/* keyboard keys, not actual characters, don't invoke with side-effect in argmuent */
#define KEY(c) ( \
	( (c) == '\033'  ) ? KEY_ESCAPE    : \
	( (c) == '\x08'  ) ? KEY_BACKSPACE : \
	( (c) == '\t'    ) ? KEY_TAB       : \
	( (c) == '\n'    ) ? KEY_RETURN    : \
	( (c) == '\r'    ) ? KEY_RETURN    : \
	( (c) <  0x20    ) ? KEY_NONE      : \
	( (c) == ' '     ) ? KEY_SPACEBAR  : \
	( (c) >= '0' && (c) <= '9' ) ? KEY_NUMBER((c)-'0') : \
	( ((c)&~0x20) >= 'A' && ((c)&~0x20) <= 'Z' ) ? KEY_ALPHA((c)-'A') : \
	KEY_NONE \
)

#define KEY_POWER_DOWN   (HID_KEY(HID_GENERIC_DESKTOP_PAGE, 0x81))
#define KEY_SLEEP        (HID_KEY(HID_GENERIC_DESKTOP_PAGE, 0x82))
#define KEY_WAKE_UP      (HID_KEY(HID_GENERIC_DESKTOP_PAGE, 0x83))
#define KEY_COLD_RESTART (HID_KEY(HID_GENERIC_DESKTOP_PAGE, 0x8e))
#define KEY_WARM_RESTART (HID_KEY(HID_GENERIC_DESKTOP_PAGE, 0x8f))

#define KEY_PLAY                (HID_KEY(HID_CONSUMER_PAGE, 0xb0))
#define KEY_PAUSE               (HID_KEY(HID_CONSUMER_PAGE, 0xb1))
#define KEY_RECORD              (HID_KEY(HID_CONSUMER_PAGE, 0xb2))
#define KEY_FAST_FORWARD        (HID_KEY(HID_CONSUMER_PAGE, 0xb3))
#define KEY_REWIND              (HID_KEY(HID_CONSUMER_PAGE, 0xb4))
#define KEY_SCAN_NEXT_TRACK     (HID_KEY(HID_CONSUMER_PAGE, 0xb5))
#define KEY_SCAN_PREVIOUS_TRACK (HID_KEY(HID_CONSUMER_PAGE, 0xb6))
#define KEY_STOP                (HID_KEY(HID_CONSUMER_PAGE, 0xb7))
#define KEY_EJECT               (HID_KEY(HID_CONSUMER_PAGE, 0xb8))

#define KEY_CALCULATOR          (HID_KEY(HID_CONSUMER_PAGE, 0x192))
#define KEY_INTERNET_BROWSER    (HID_KEY(HID_CONSUMER_PAGE, 0x196))

#define KEY_MUTE                (HID_KEY(HID_KEYBOARD_PAGE, 0x7F))
#define KEY_VOLUME_UP           (HID_KEY(HID_KEYBOARD_PAGE, 0x80))
#define KEY_VOLUME_DOWN         (HID_KEY(HID_KEYBOARD_PAGE, 0x81))

#define KEY_TOGGLE_FULL_SCREEN  (HID_KEY(HID_CONSUMER_PAGE, 0x232))

#define KEY_INCREASE_BRIGHTNESS (HID_KEY(HID_CONSUMER_PAGE, 0x6f))
#define KEY_DECREASE_BRIGHTNESS (HID_KEY(HID_CONSUMER_PAGE, 0x70))

#define KEY_ZOOM_IN             (HID_KEY(HID_CONSUMER_PAGE, 0x22d))
#define KEY_ZOOM_OUT            (HID_KEY(HID_CONSUMER_PAGE, 0x22e))

#endif // HID_KEYDEF_H
