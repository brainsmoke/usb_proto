
use <../lib/utils.scad>

use <pcb.scad>

include <../lib/case_base.scad>
include <../lib/case_pcb_standard.scad>

/* overrides */

grid_origin = pcb_center();
grid_rows = 2;

has_buttons=false;
has_dfu_button=true;
has_leds=false;

total_height=7;
screw_guaranteed_depth=6;
leg_height =1.8;
bottom_thickness = .6;
top_thickness = .6;

head_diameter = 5.54;
head_thickness = 1.86;
screw_clearance = .2;

grid_height_bottom=.6;
grid_height_top=1.4;


chamfer=0;
