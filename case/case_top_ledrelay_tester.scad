
include <case_base.scad>

use <graft.scad>

module at_buttons()
{   
	at_back()
		translate([-4*2.54, -pcb_radius, total_height])
			children();

	at_dfu_button() children();
}

tube_d=5.;
tube_width_base=1.6;
tube_width_top=.8;
tube_height=5;

module cable_tube()
{
	graft()
	translate([ 0, 20, total_height-tube_height])
	{
		graft_add()
		cylinder(tube_height-e, tube_d/2+tube_width_top, tube_d/2+tube_width_base);
		graft_remove()
		translate([0,0,-b])
		cylinder(8+2*b, r=tube_d/2);
	}
}

flip() top()
{
	top_features();

	for (x=[45,55])
	translate([x, 10, 0])
	cable_tube();
}
