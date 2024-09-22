
PROJECTS=simple

BOARDHOUSE=jlc
PARTNO_MARKING=JLCJLCJLCJLC

BUILDDIR=build/%
TMPDIR=tmp/%
PCB=pcb/%/project.kicad_pcb
SCHEMATIC=pcb/%/project.kicad_sch
POSFILE_KICAD=$(TMPDIR)/posfile_top_kicad.csv
POSFILE=$(BUILDDIR)/posfile_$(BOARDHOUSE).csv
ZIPFILE=$(BUILDDIR)/gerbers_$(BOARDHOUSE).zip
BOMFILE=$(BUILDDIR)/bomfile_$(BOARDHOUSE).csv
DRILLFILES=$(TMPDIR)/project-NPTH.drl $(TMPDIR)/project-PTH.drl

LAYERS2=F.Cu B.Cu F.Mask B.Mask F.Paste B.Paste F.Silkscreen B.Silkscreen Edge.Cuts
LAYERS4=$(LAYERS2) In1.Cu In2.Cu

LAYERS :=$(LAYERS2)
COMMA :=,
SPACE :=$() $()

GERBERS := $(foreach layer, $(subst .,_, $(LAYERS)), $(TMPDIR)/project-$(layer).gbr)

GERBER_EXPORT_LIST=$(subst $(SPACE),$(COMMA),$(value LAYERS))


GERBER_OPTS=--no-protel-ext --board-plot-params -D PCB_ORDER_NUMBER="$(PARTNO_MARKING)"
DRILL_OPTS=--format=excellon --excellon-oval-format=route --excellon-separate-th
BOM_JLC_OPTS=--fields='Value,Reference,Footprint,LCSC' --labels='Comment,Designator,Footprint,JLCPCB Part \#' --group-by=LCSC --ref-range-delimiter='' --exclude-dnp

BOM_OPTS=$(BOM_JLC_OPTS)
POS_OPTS=--exclude-dnp --side front --units=mm --format=csv

SCAD_DEPS=case/case.scad case/usb.scad
CASE=$(BUILDDIR)/case_bottom.stl \
     $(BUILDDIR)/case_top.stl

CASE_EXTRA=

TMPFILES=$(GERBERS) $(DRILLFILES) $(POSFILE_KICAD)
EXTRA=$(CASE_EXTRA)


PROJECT_TARGETS=$(PROJECTS:=.project)
TARGETS=$(PROJECT_TARGETS)

BUILD_FILES=$(foreach project, $(PROJECTS), \
            $(patsubst %, $(POSFILE_KICAD), $(project)) \
            $(patsubst %, $(POSFILE), $(project)) \
            $(foreach gerber, $(GERBERS), $(patsubst %, $(gerber), $(project))) \
            $(foreach drillfile, $(DRILLFILES), $(patsubst %, $(drillfile), $(project))))

.PHONY: all extra clean $(PROJECT_TARGETS)

all: $(TARGETS)

$(PROJECT_TARGETS): %.project: $(ZIPFILE) $(POSFILE) $(BOMFILE)

extra: $(TARGETS) $(EXTRA)

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

$(BUILDDIR)/%.stl: case/%.scad $(SCAD_DEPS)
	mkdir -p $(dir $@)
	openscad -o $@ $<

clean:
	-rm $(BUILD_FILES)
