
include <case_base.scad>

grid_rows = 10;

preview()
{
	pcb();
	on_pcb()
	{
		at_front() usb_c();
	}
}

module at_extra_holes()
{
	diag = sqrt( hole_dist_x*hole_dist_x + hole_dist_y*hole_dist_y );

	for (y = [hole_dist_y/2-diag/2, hole_dist_y/2+diag/2])
		translate([hole_dist_x/2, y,0])
			children();
}

module at_holes()
{
	for (x = [0, hole_dist_x])
		for (y = [0, hole_dist_y])
			translate([x,y,0])
				children();

	at_extra_holes() children();
}

case();

next() next() flip() top()
{
	top_features();

	at_extra_holes()
	graft_add()
	intersection()
	{
		screw_shape(leg_thickness);
		translate([0,0,bottom_thickness+leg_height+e*2])
		cylinder(component_z-bottom_thickness+leg_height, r=max(screw_loose_radius,funnel_top_inner_radius)+leg_thickness+1);
	}
}



