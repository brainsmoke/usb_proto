e=.001;

use <graft.scad>;
use <utils.scad>;

module pinheader_case_hole(depth, cols, rows=1, pitch=2.54, border=.8, margin=.25, $fn=20)
{
	graft()
	{
		m2 = margin*2;
		b2 = border*2;
		dim = [cols*pitch+m2, rows*pitch+m2, depth];

		graft_remove()
		translate([0,0,e])
		rounded_block_z(dim + [0,0,2*e], anchor =[0,0,1], r=margin);

		graft_add()
		translate([0,0,-e])
		rounded_block_z(dim + [b2, b2,-e], anchor =[0,0,1], r=margin+border );
	}
}

pinheader_case_hole(10, 8, 1);
translate([30,0,0])
pinheader_case_hole(10, 1, 1, margin=0);

