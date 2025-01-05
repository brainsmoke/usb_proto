
PROJECTS=simple simple-v0.1 mcu

BOARDHOUSE=jlc
PARTNO_MARKING=JLCJLCJLCJLC
BOM_JLC_OPTS=--fields='Value,Reference,Footprint,LCSC' --labels='Comment,Designator,Footprint,JLCPCB Part \#' --group-by=LCSC --ref-range-delimiter='' --exclude-dnp

REQUIRE_DRC=y

PCB_VARIABLES=-D PCB_ORDER_NUMBER="$(PARTNO_MARKING)"
GERBER_OPTS=--no-protel-ext --board-plot-params $(PCB_VARIABLES)
DRC_OPTS=--exit-code-violations $(PCB_VARIABLES)
DRILL_OPTS=--format=excellon --excellon-oval-format=route --excellon-separate-th
BOM_OPTS=$(BOM_JLC_OPTS)
POS_OPTS=--exclude-dnp --side front --units=mm --format=csv

LAYERS2=F.Cu B.Cu F.Mask B.Mask F.Paste B.Paste F.Silkscreen B.Silkscreen Edge.Cuts
LAYERS4=$(LAYERS2) In1.Cu In2.Cu
LAYERS :=$(LAYERS2)

SCAD_DIR=case
SCAD_DEPS=case/case_base.scad case/usb.scad case/button.scad
SCAD_PARAM_DIR=case/parameters
SCAD_PARAM_SET=default
SCAD_PARTS=case case_bottom case_top case_top_plain
SCAD_DEFINES=

include rules.mk

