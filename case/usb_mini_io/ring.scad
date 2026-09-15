key_pitch = 20;
n_keys = 12;
r_keys = (key_pitch/2)/tan(360/n_keys/2) + key_pitch/2;

include <case_pcb.scad>
use <../lib/keyswitch.scad>

total_height=19;


chamfer_top=4;
chamfer_bottom=1.8;

has_dfu_button=false;

grid_pitch=key_pitch/2;
grid_width=.8;

panel_dim = key_pitch-0.8;
panel_thickness = 1.5;

grid_height_bottom=1.6;
grid_height_top=0;


pcb_pos = [ -r_keys - key_pitch/2, -10, top_component_z ];

r_outer = sqrt(pow(r_keys+key_pitch/2,2)+pow(10,2));
r_inner = (r_keys-key_pitch/2); /* /cos(360/n_keys/2)+1.; */

module case_shape(height, radius)
{
	linear_extrude(height)
	difference()
	{
		circle(r=r_outer+radius, $fn=200);
		circle(r=r_inner-radius, $fn=200);
	}
}

module case_shape_chamfer_convex(height, r1, r2)
{
	difference()
	{
		cylinder(height,r1=r_outer+r1,r2=r_outer+r2, $fn=200);
		cylinder(height,r1=r_inner-r1,r2=r_inner-r2, $fn=200);
	}
}


module at_keys()
{
	for (i = [ 0 : n_keys-1 ])
	rotate( [0,0,i*360/n_keys] )
	translate([r_keys,0,total_height])
	children();
}

module at_extra_holes()
{
	for (a=[90,180,270])
	rotate([0,0,a])
	translate([-r_keys-key_pitch/2, key_pitch/2,0])
	children();
}

module next()
{
	translate([2*r_outer+outer_radius*2+padding,0,0])
	children();
}

module flip()
{
	rotate([0,180,0])
	translate([0,0,-total_height])
	children();
}

module top_features()
{
	at_keys()
	keyswitch_flush(panel_dim);
}

module mockup()
{
	preview()
	{
		on_pcb()
		pcb();

		at_keys()
		at_mx_flush()
		mx_keepout();
	}
}

bottom();
mockup();

next() flip()
{
	top() top_features();
	mockup();
}
