
PROJECTS=simple usbserial

BOARDHOUSE=jlc
PARTNO_MARKING=JLCJLCJLCJLC
BOM_JLC_OPTS=--fields='Value,Reference,Footprint,LCSC' --labels='Comment,Designator,Footprint,JLCPCB Part \#' --group-by=LCSC --ref-range-delimiter='' --exclude-dnp

GERBER_OPTS=--no-protel-ext --board-plot-params -D PCB_ORDER_NUMBER="$(PARTNO_MARKING)"
DRILL_OPTS=--format=excellon --excellon-oval-format=route --excellon-separate-th
BOM_OPTS=$(BOM_JLC_OPTS)
POS_OPTS=--exclude-dnp --side front --units=mm --format=csv

CASE_PARAM_SET=$*

LAYERS2=F.Cu B.Cu F.Mask B.Mask F.Paste B.Paste F.Silkscreen B.Silkscreen Edge.Cuts
LAYERS4=$(LAYERS2) In1.Cu In2.Cu
LAYERS :=$(LAYERS2)

BUILDDIR=build/%
TMPDIR=tmp/%
PCB=pcb/%/project.kicad_pcb
SCHEMATIC=pcb/%/project.kicad_sch
POSFILE_KICAD=$(TMPDIR)/posfile_top_kicad.csv
POSFILE=$(BUILDDIR)/posfile_$(BOARDHOUSE).csv
ZIPFILE=$(BUILDDIR)/gerbers_$(BOARDHOUSE).zip
BOMFILE=$(BUILDDIR)/bomfile_$(BOARDHOUSE).csv
DRILLFILES=$(TMPDIR)/project-NPTH.drl $(TMPDIR)/project-PTH.drl

COMMA :=,
SPACE :=$() $()
GERBER_EXPORT_LIST=$(subst $(SPACE),$(COMMA),$(value LAYERS))

GERBERS := $(foreach layer, $(subst .,_, $(LAYERS)), $(TMPDIR)/project-$(layer).gbr)

SCAD_PARAMETERS=case/parameters.json
SCAD_DEPS=case/case.scad case/usb.scad $(SCAD_PARAMETERS)
CASES=$(BUILDDIR)/case.stl \
      $(BUILDDIR)/case_bottom.stl \
      $(BUILDDIR)/case_top.stl

TMPFILES=$(GERBERS) $(DRILLFILES) $(POSFILE_KICAD)

PROJECT_TARGETS=$(PROJECTS:=.project)
TARGETS=$(PROJECT_TARGETS)

INTERMEDIATE_FILES=$(foreach project, $(PROJECTS), \
            $(patsubst %, $(POSFILE_KICAD), $(project)) \
            $(foreach gerber, $(GERBERS), $(patsubst %, $(gerber), $(project))) \
            $(foreach drillfile, $(DRILLFILES), $(patsubst %, $(drillfile), $(project))))

BUILD_FILES=$(foreach project, $(PROJECTS), \
            $(patsubst %, $(POSFILE), $(project)) \
            $(patsubst %, $(BOMFILE), $(project)) \
            $(patsubst %, $(ZIPFILE), $(project)) \
            $(foreach case, $(CASES), $(patsubst %, $(case), $(project))))

.PHONY: all clean $(PROJECT_TARGETS)

all: $(TARGETS)

$(PROJECT_TARGETS): %.project: $(ZIPFILE) $(POSFILE) $(BOMFILE) $(CASES)

$(GERBERS): $(PCB)
	mkdir -p $(dir $@)
	kicad-cli pcb export gerbers $(GERBER_OPTS) -o $(dir $@) --layers=$(GERBER_EXPORT_LIST) $<

$(POSFILE_KICAD): $(PCB)
	mkdir -p $(dir $@)
	kicad-cli pcb export pos $< $(POS_OPTS) -o $@

$(BOMFILE): $(SCHEMATIC)
	mkdir -p $(dir $@)
	kicad-cli sch export bom -o $@ $(BOM_OPTS) $<

$(DRILLFILES): $(PCB)
	mkdir -p $(dir $@)
	kicad-cli pcb export drill $(DRILL_OPTS) -o $(dir $@) $<

$(POSFILE): $(POSFILE_KICAD)
	mkdir -p $(dir $@)
	python3 script/posfile_to_boardhouse.py $(BOARDHOUSE) < $< > $@

$(ZIPFILE): $(GERBERS) $(DRILLFILES)
	mkdir -p $(dir $@)
	zip -o - -j $^ > $@

$(BUILDDIR)/case.stl: case/case.scad $(SCAD_DEPS)
	mkdir -p $(dir $@)
	openscad -o $@ -p $(SCAD_PARAMETERS) -P $(CASE_PARAM_SET) $<

$(BUILDDIR)/case_top.stl: case/case_top.scad $(SCAD_DEPS)
	mkdir -p $(dir $@)
	openscad -o $@ -p $(SCAD_PARAMETERS) -P $(CASE_PARAM_SET) $<

$(BUILDDIR)/case_bottom.stl: case/case_bottom.scad $(SCAD_DEPS)
	mkdir -p $(dir $@)
	openscad -o $@ -p $(SCAD_PARAMETERS) -P $(CASE_PARAM_SET) $<

clean:
	-rm $(INTERMEDIATE_FILES) $(BUILD_FILES)
