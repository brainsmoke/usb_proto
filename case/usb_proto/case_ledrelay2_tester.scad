
include <case_pcb.scad>

use <../lib/graft.scad>
use <../lib/pinheader.scad>

has_buttons=false;
has_leds=false;

total_height=12;

hole_d=4.;
hole_width_base=.8;
hole_width_top=.8;
hole_height=total_height-top_component_z-2.5;

chamfer = 1;

module led_hole(h, d, width_top, width_base)
{
	graft()
	translate([0,0,total_height-h])
	{
		graft_add()
		cylinder(h-e, d/2+width_top, d/2+width_base);
		graft_remove()
		translate([0,0,-b])
		cylinder(8+2*b, r=d/2);
	}
}

module mockup()
{
	preview()
	on_pcb()
	pcb();
}
 
bottom();
mockup();
 
next() flip() top()
{
	top_features();

	for (y=[7,10])
	translate(breadboard_pos(5.5,y))
	at_top_above_pcb()
	socketheader_case_hole(total_height-top_component_z-1, 4, 1);

	for (x=[14,19])
	translate(breadboard_pos(x,8.5))
	led_hole(hole_height, hole_d, hole_width_top, hole_width_base);

};

