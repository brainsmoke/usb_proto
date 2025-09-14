
PROJECTS=simple simple-v0.1 mcu
PCBA=simple simple-v0.1 mcu

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
mcu/case \
mcu/case_bottom \
mcu/case_top \
mcu/case_top_plain \
mcu/case_bottom_ledrelay_tester \
mcu/case_top_ledrelay_tester \
mcu/case_top_ledrelay_tester_v2 \
mcu/case_almost_regular_hexagon \
mcu/case_top_keyswitches \
mcu/case_top_keyswitches_8 \
mcu/case_top_led_connector

SCAD_DEFINES=

include rules.mk

