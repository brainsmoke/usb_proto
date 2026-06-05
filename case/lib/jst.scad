e=.001;

use <graft.scad>;

module jst_sm(n=3, margin=0)
{
	h=11.7 + margin;
	w=n*2.5+.5 + 2*margin;
	d=5 + 2*margin;
	y_off = 1;
	top_notch_h = 1+2*margin;
	top_notch_z = h-1-2*margin;
	top_notch_d = d+2.4;

	bottom_notch_h = 1+margin;
	bottom_notch_w = 3.5+2*margin;
	bottom_notch_d = 1;

	grip_d = 4+margin*2;
	grip_w = 3;
	grip_h = 6;
	grip_z = 4;

	color([.3,.3,.3])
	{
		translate([-w/2, -d/2-y_off, 0])
		cube([w, d, h]);

		translate([-bottom_notch_w/2, d/2-y_off, 0])
		cube([bottom_notch_w, bottom_notch_d, bottom_notch_h]);

		translate([-w/2, -top_notch_d/2-y_off, top_notch_z])
		cube([w, top_notch_d, top_notch_h]);

		translate([0,-y_off,0])
		for (zrot=[0,180])
		rotate([0,0,zrot])
		translate([-w/2,-grip_d/2,grip_z])
		rotate([-90,180,0])
		linear_extrude(grip_d)
		polygon([[0,0], [-e,0], [-e, grip_h], [grip_w/2, grip_h], [grip_w, grip_h/2]]);

	}
}


module _jst_sm_cavity(n, margin, z)
{
	minkowski()
	{
		jst_sm(n, margin);
		translate([e/2,e/2,-z])
		cube([e,e,z]);
	}
}

module jst_sm_case_hole(n, margin, z, b)
{
	graft()
	{
		graft_add()
		{
			difference()
			{
				_jst_sm_cavity(n, margin+b, z);

				w_over = (10+n*2.5);
				d_over = 20;
				h_over = 20;
				translate([-w_over/2, -d_over/2, -e])
				cube([w_over, d_over, h_over]);
			}
		}
		graft_remove()
		{
			_jst_sm_cavity(n, margin, z+e);
		}
	}

}


translate([20,0,0])
jst_sm(3, 0);

translate([40,0,0])
jst_sm(3, .3);

jst_sm_case_hole(3, .3, 14, .8);
