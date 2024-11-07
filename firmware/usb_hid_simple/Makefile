
OPENCM3_DIR=../libopencm3
SHARED_DIR=../common
PROJECT=usb_hid_simple
CFILES=main.c usb_hid_keypad.c util.c

DFU_FILE=$(PROJECT).dfu

TOOLS_DIR=../../tools
ELF_TO_DFUSE=$(TOOLS_DIR)/elf_to_dfuse.py

DFU_ID_VENDOR=0483
DFU_ID_PRODUCT=df11
DFU_UTIL=dfu-util

# target chip
DEVICE=stm32f042g6u6

%.dfu: %.elf
	python3 $(ELF_TO_DFUSE) $(DFU_ID_VENDOR):$(DFU_ID_PRODUCT) $< $@

default: all $(DFU_FILE)

INCLUDES += $(patsubst %,-I%, . $(SHARED_DIR))
VPATH += $(SHARED_DIR)

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk
include ../rules.mk

dfu_flash: $(DFU_FILE)
	$(DFU_UTIL) -a 0 -D $<
	

