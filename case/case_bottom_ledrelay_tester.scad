
include <case_base.scad>

use <graft.scad>

tube_d=5.;
tube_width_base=1.6;
tube_width_top=.8;
tube_height=5;
total_height=18;

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

case();
