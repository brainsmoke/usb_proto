#!/bin/bash

(cat <<EOF
set mem inaccessible-by-default off
set confirm off
target extended-remote /dev/ttyACM0
monitor swdp_scan
attach 1
load
EOF
) > tmp.txt

gdb-multiarch -x tmp.txt usb_serial.elf
