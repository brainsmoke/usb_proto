e=.001;

use <graft.scad>;
use <utils.scad>;

module pinheader_case_hole(depth, cols, rows=1, pitch=2.54, border=.8, margin=.25, end_width=0, $fn=20)
{
	graft()
	{
		dim = [cols*pitch+margin*2+end_width*2, rows*pitch+margin*2, depth];

		graft_remove()
		translate([0,0,e])
		rounded_block_z(dim + [0,0,2*e], anchor =[0,0,1], r=margin);

		graft_add()
		translate([0,0,-e])
		rounded_block_z(dim + [border*2, border*2,-e], anchor =[0,0,1], r=margin+border );
	}
}

module socketheader_case_hole(depth, cols, rows=1, pitch=2.54, border=.8, margin=.25, end_width=.25, $fn=20)
{
	pinheader_case_hole(depth, cols, rows, pitch, border, margin, end_width, $fn);
}

pinheader_case_hole(10, 8, 1);
translate([30,0,0])
socketheader_case_hole(10, 2, 1);

