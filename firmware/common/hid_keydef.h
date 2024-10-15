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

/* key definitions from: https://www.usb.org/sites/default/files/hut1_4.pdf */

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

#define KEY_A                   (HID_KEY(HID_KEYBOARD_PAGE, 0x04))
#define KEY_B                   (HID_KEY(HID_KEYBOARD_PAGE, 0x05))
#define KEY_C                   (HID_KEY(HID_KEYBOARD_PAGE, 0x06))
#define KEY_D                   (HID_KEY(HID_KEYBOARD_PAGE, 0x07))

#endif // HID_KEYDEF_H
