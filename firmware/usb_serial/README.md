# usb serial to uart

```
make -C ../libopencm3/ # if not done already
make

# using DFU
# push DFU boot button while inserting the device
make dfu_flash

# using black magic probe:

gdb-multiarch usb_serial.elf
set mem inaccessible-by-default off
target extended-remote /dev/ttyACM0
monitor swdp_scan
attach 1
load
run
```

