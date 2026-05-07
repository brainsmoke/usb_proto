
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

bottom();

next() next() flip() top()
{
	top_features();
}



