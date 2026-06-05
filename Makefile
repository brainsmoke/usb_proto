
PROJECTS=simple simple-v0.1 usb_proto usb_mini_io
PCBA=simple simple-v0.1 usb_proto usb_mini_io

BASENAME=project
BOARDHOUSE=jlc
PARTNO_MARKING=JLCJLCJLCJLC
BOM_JLC_OPTS=--fields='Value,Reference,Footprint,LCSC' --labels='Comment,Designator,Footprint,JLCPCB Part \#' --group-by=LCSC --ref-range-delimiter='' --exclude-dnp

REQUIRE_DRC=y

PCB_VARIABLES=-D PCB_ORDER_NUMBER="$(PARTNO_MARKING)"
GERBER_OPTS=--no-x2 --no-netlist --no-protel-ext $(PCB_VARIABLES)
DRC_OPTS=--exit-code-violations $(PCB_VARIABLES)
DRILL_OPTS=--format=excellon --excellon-oval-format=route --excellon-separate-th
BOM_OPTS=$(BOM_JLC_OPTS)
POS_OPTS=--exclude-dnp --side front --units=mm --format=csv

LAYERS2=F.Cu B.Cu F.Mask B.Mask F.Paste B.Paste F.Silkscreen B.Silkscreen Edge.Cuts
LAYERS4=$(LAYERS2) In1.Cu In2.Cu
LAYERS :=$(LAYERS4)

SCAD_DIR=case
SCAD_DEPS=case/case_base.scad case/usb.scad case/button.scad
SCAD_PARAM_DIR=case/parameters
SCAD_PARAM_SET=default
SCAD_PARTS=simple/case simple-v0.1/case \
usb_proto/case \
usb_proto/case_bottom \
usb_proto/case_top \
usb_proto/case_top_plain \
usb_proto/case_bottom_ledrelay_tester \
usb_proto/case_top_ledrelay_tester \
usb_proto/case_top_ledrelay_tester_v2 \
usb_proto/case_almost_regular_hexagon \
usb_proto/case_top_keyswitches \
usb_proto/case_top_keyswitches_8 \
usb_proto/case_top_led_connector \
usb_mini_io/keypad \
usb_mini_io/old_keypad \
usb_mini_io/game_controller \
usb_mini_io/single_key

SCAD_DEFINES=

include rules.mk

