
use <../lib/utils.scad>

use <pcb.scad>

include <../lib/case_base.scad>
include <../lib/case_pcb_standard.scad>

/* overrides */

grid_origin = pcb_center();
grid_rows = 4;
grid_cols = 6;

has_buttons=true;
has_dfu_button=true;
has_leds=true;

chamfer=0;

