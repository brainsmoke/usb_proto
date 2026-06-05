
key_pitch = 20;

controller_width=140;
controller_depth = 60;

include <case_pcb.scad>

use <../lib/keyswitch.scad>

pcb_pos = [-controller_depth/2+outer_radius,-10, top_component_z];

chamfer = 1.2;

total_height=17;

keycenter_off = controller_width/2-controller_depth/2;

has_dfu_button=true;

grid_pitch=key_pitch/2;
grid_width=.8;

leg_height=6;

panel_dim = key_pitch-0.8;

module at_keycenters()
{
	for (r=[0,180])
	rotate([0,0,r])
	translate([0,keycenter_off,0])
	children();
}

module case_shape(height, radius)
{
	hull()
	at_keycenters()
	cylinder(height, r=controller_depth/2-outer_radius+radius, $fn=100);
}

module at_extra_holes()
{
	at_keycenters()
	for(r=[0,90])
	rotate([0,0,r])
	translate([key_pitch*3/4,key_pitch*3/4,0])
	children();

	translate([5,0,0])
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
	translate([controller_depth/2-12,y,total_height])
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
	top()
	{
		top_features();
		at_keys()
		keyswitch_flush(panel_dim);
	}
	mockup();
}
