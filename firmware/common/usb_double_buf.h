#ifndef USB_DOUBLE_BUF
#define USB_DOUBLE_BUF


#include <libopencm3/usb/usbd.h>

void usb_double_buffer_endpoint_setup(usbd_device *device, uint8_t endpoint, uint16_t max_size);

/* these also swap buffers when possible */
int usb_double_buffer_can_read(uint8_t endpoint);
int usb_double_buffer_can_write(uint8_t endpoint);

uint16_t usb_double_buffer_read_packet(uint8_t endpoint, uint8_t *buf, uint16_t len);
uint16_t usb_double_buffer_write_packet(uint8_t endpoint, const uint8_t *buf, uint16_t len);

/* optional callbacks */
void usb_double_buffer_data_available_cb(uint8_t endpoint);
void usb_double_buffer_can_write_cb(uint8_t endpoint);

#endif // USB_DOUBLE_BUF
