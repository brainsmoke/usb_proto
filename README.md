# USB-C connected boards with programmable USB connectivity

This repository contains a few boards aimed at rapid prototyping.

The main idea is to save time by providing often-used components
and a microcontroller that can act as a USB-serial to UART
(usb-cdc-acm) device, a HID keyboard/keypad or run other simple
programs.

# PCBs

## A protoboard with an STM32F042 for usb connectivity & GPIO / UART functionality.

Production files: [gerbers](build/mcu/gerbers_jlc.zip) [BOM](build/mcu/bomfile_jlc.csv) [POS](build/mcu/posfile_jlc.csv)

Cases: [basic](build/mcu/case.stl) [hexagon](build/mcu/case_almost_regular_hexagon.stl) [keyswitches](build/mcu/case_top_keyswitches_8.stl) [led strips](build/mcu/case_top_led_connector.stl)

![Image of a USB-C connected protoboard](img/protoboard_mcu.jpg)

* 5v USB-C fed (with 5k1 resistors for true USB-C compatibility)
* 3v3 linear regulator
* STM32F042G6 microcontroller (with xtal-less 12mbit USB support)
* A button to boot the STM32 in DFU mode
* GPIO Port A pins 0 to 7 broken out
* GPIO Port B pins 6 & 7 broken out (for UART TX/RX)
* 4 general purpose LEDs
* 3 general purpose buttons
* A 21 column breadboard-style protoboard
* size: 47x77mm
* 4 M3 holes 40mm and 70mm apart

## An STM32F042 breakout board

Production files: [gerbers](build/mcu_tiny/gerbers_jlc.zip) [BOM](build/mcu_tiny/bomfile_jlc.csv) [POS](build/mcu_tiny/posfile_jlc.csv)

![Image of a panel with 6 breakout boards](img/mcu_tiny.jpg)

* for smaller enclosures (27x22mm)
* I/O & firmware compatible with the protoboard
* 5v USB-C fed (with 5k1 resistors for true USB-C compatibility)
* 3v3 linear regulator
* STM32F042G6 microcontroller (with xtal-less 12mbit USB support)
* A button to boot the STM32 in DFU mode
* no LEDs / general purpose buttons / protoboard space
* 2 M3 holes 20mm apart

## (old) USB-C Fed Protoboard

The first version of the protoboard did not have a microcontroller:
[pcb/simple](pcb/simple)

# Firmware (still untested!)

* [usb_serial](firmware/usb_serial) USB serial to UART adapter firmware, with RTS/DTR signals & reset/boot signals for directly programming ESP32s.
* [usb_hid_keymatrix](firmware/usb_hid_keymatrix) USB HID Keypad firmware, for a keyboard matrix
* [usb_hid_simple](firmware/usb_hid_simple) USB HID Keypad firmware for a few buttons

# License

* The hardware (schematic / PCB / production files / case models ) is Licensed CC-BY-4.0
* The firmware (in the firmware directory) uses libopencm3 which is licensed LGPLv3 and
  all code specific to this project is distributed under the MIT license.

