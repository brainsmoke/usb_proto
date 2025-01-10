
include <case_base.scad>

use <sculpt.scad>

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
	sculpt()
	translate([ 0, 20, total_height-tube_height])
	{
		sculpt_add()
		cylinder(tube_height-e, tube_d/2+tube_width_top, tube_d/2+tube_width_base);
		sculpt_carve()
		translate([0,0,-e])
		cylinder(8+2*e, r=tube_d/2);
	}
}

flip() top()
{
	top_features();

	for (x=[15,25])
	translate([x, 10, 0])
	cable_tube();
}
