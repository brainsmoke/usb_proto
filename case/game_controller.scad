
key_pitch = 20;

controller_width=140;

include <case_tiny_base.scad>
use <keyswitch.scad>

total_height=17;
hole_dist_x = 50;

keycenter_off = controller_width/2-hole_dist_x/2-outer_radius;

has_dfu_button=true;

grid_rows=14;
grid_cols=6;
grid_pitch=key_pitch/2;
grid_x_off=0;
grid_width=.8;

panel_dim = key_pitch-0.8;

bounding_box = [ [            -outer_radius, hole_dist_y/2-controller_width/2, 0           ],
                 [ hole_dist_x+outer_radius, hole_dist_y/2+controller_width/2, total_height] ];


module at_keycenters()
{
	translate([hole_dist_x/2,hole_dist_y/2,0])
	for (r=[0,180])
	rotate([0,0,r])
	translate([0,keycenter_off,0])
	children();
}

module case_shape(height, radius)
{
	hull()
	at_keycenters()
	cylinder(height, r=hole_dist_x/2+radius);
}

module at_extra_holes()
{
	at_keycenters()
	for(r=[0,90])
	rotate([0,0,r])
	translate([key_pitch*3/4,key_pitch*3/4,0])
	children();

	translate([30,hole_dist_y/2,0])
	children();
}

module at_keys()
{
	at_keycenters()
	translate([0,0,total_height])
	for(r=[45,135,-135,-45])
	rotate([0,0,r])
	translate([-key_pitch/2,-key_pitch/2,0])
	children();

	for (y=[-key_pitch/2,key_pitch/2])
	translate([hole_dist_x-7,y+hole_dist_y/2,total_height])
	children();
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
	top()
	{
		top_features();
		at_keys()
		keyswitch_flush(panel_dim);
	}
	mockup();
}
