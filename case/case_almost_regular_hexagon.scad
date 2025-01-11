
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

module at_holes(x, y)
{
	for (x = [0, hole_dist_x])
		for (y = [0, hole_dist_y])
			translate([x,y,0])
				children();
	mid_len = sqrt( hole_dist_x*hole_dist_x + hole_dist_y*hole_dist_y );

	for (y = [hole_dist_y/2-mid_len/2, hole_dist_y/2+mid_len/2])
		translate([hole_dist_x/2, y,0])
			children();

}

case();

next() next() flip() top() top_features();


