
DEPS=case.scad usb.scad grid.scad
TARGETS=case.stl case_bottom.stl case_top.stl case_orig.stl

all: $(TARGETS)

%.stl: %.scad $(DEPS)
	openscad -o $@ $<

clean:
	-rm $(TARGETS)
