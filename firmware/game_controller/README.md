# Game controller

![A case resembling a game controller, but all the controls are cherry mx style keyswitches](/img/game_controller.jpg)

# schematic

![buttons in pull-down configuration](circuit.png)

# 3D printed case:

[game controller](/build/mcu_tiny/game_controller.stl)

# build instructions

Edit keys in `main.c`

```
git submodule init
git submodule update
make -C ../libopencm3/ # if not done already
make


# using DFU
# push DFU boot button while inserting the device
make dfu_flash

# using black magic probe:

gdb-multiarch usb_hid_keypad.elf
set mem inaccessible-by-default off
set confirm off
target extended-remote /dev/ttyACM0
monitor swdp_scan
attach 1
load
run
```

