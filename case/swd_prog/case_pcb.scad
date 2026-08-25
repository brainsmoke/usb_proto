
use <../lib/pinheader.scad>
use <../lib/utils.scad>

use <pcb.scad>

include <../lib/case_base.scad>
include <../lib/case_pcb_standard.scad>

/* overrides */

grid_origin = pcb_center();
grid_rows = 2;

has_buttons=false;
has_dfu_button=true;
has_leds=true;

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

module swd_header()
{
	at_top_above_pcb()
	translate([30, 10, 0])
	pinheader_case_hole(total_height-top_component_z, 1, 5);
}

module swd_header_simple()
{
	at_top_above_pcb()
	translate([30, 10, 0])
	pinheader_case_hole(total_height-top_component_z, 1, 3);
}

module jtag_header()
{
	at_top_above_pcb()
	translate([30, 10, 0])
	translate([-2.54*2,0,0])
	pinheader_case_hole(total_height-top_component_z, 1, 7);
}

module serial_header()
{
	at_top_above_pcb()
	translate([30, 10, 0])
	translate([-2.54*4,2.54*3,0])
	pinheader_case_hole(total_height-top_component_z, 1, 3);
}

module uart_header()
{
	at_top_above_pcb()
	translate([30, 10, 0])
	translate([-2.54*6.5,2.54*4,0])
	pinheader_case_hole(total_height-top_component_z, 2, 1);
}

