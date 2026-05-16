
key_pitch = 20;

include <case_tiny_base.scad>
use <keyswitch.scad>

hole_dist_x = 50;

total_height=17;

has_dfu_button=false;

grid_rows=14;
grid_cols=6;
grid_pitch=key_pitch/2;
grid_x_off=0;
grid_width=1.2;

panel_dim = key_pitch-0.8;
panel_thickness = 1.5;

pod_d=60;

module case_shape(height, radius)
{
	hull()
	for (y=[-40,40])
	translate([25,y+hole_dist_y/2,0])
	cylinder(height, r=pod_d/2-outer_radius+radius);
}

module at_extra_holes()
{
	for (y=[-40,40])
	translate([25,y+hole_dist_y/2,0])
	for(r=[0,90,180,270])
	rotate([0,0,r])
	translate([-key_pitch*3/4,-key_pitch*3/4,0])
	children();
}

module at_keys()
{
	for (y=[-40,40])
	translate([25,y+hole_dist_y/2,total_height])
	for(r=[45,135,-135,-45])
	rotate([0,0,r])
	translate([-key_pitch/2,-key_pitch/2,0])
	children();

	for (y=[-key_pitch/2,key_pitch/2])
	translate([43,y+hole_dist_y/2,total_height])
	children();
}

module top_features()
{
	at_keys()
	keyswitch_flush(panel_dim);
}

module next ()
{
	translate([70,0,0])
	children();
}

module mockup()
{
	preview()
	{
		pcb();
		on_pcb()
		{
			at_front()
			usb_c();
		}

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
