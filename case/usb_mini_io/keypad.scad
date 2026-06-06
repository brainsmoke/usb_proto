
key_pitch = 20;
n_cols = 1;
n_rows = 4;

include <case_pcb.scad>
use <../lib/keyswitch.scad>

total_height=19;

/*
chamfer_top=2.2;
chamfer_bottom=.6;
*/
chamfer_top=0;

has_dfu_button=false;

grid_pitch=key_pitch/2;
grid_width=.8;

panel_dim = key_pitch-0.8;
panel_thickness = 1.5;

grid_height_bottom=1.6;

pcb_pos = [-10, n_rows*key_pitch/2, top_component_z ];
pcb_angle = -90;

module at_keys()
{
	for (x = [ 0 : n_cols-1 ])
	for (y = [ 0 : n_rows-1 ])
	translate([(x-(n_cols-1)/2)*key_pitch,(y-(n_rows-1)/2)*key_pitch,total_height])
	children();
}

module at_extra_holes()
{
	y_range = (n_cols == 1) ? [-n_rows*key_pitch/2] : [-n_rows*key_pitch/2, n_rows*key_pitch/2];
	x_range = [-n_cols*key_pitch/2, n_cols*key_pitch/2];

	for (x = x_range)
	for (y = y_range)
	translate([x,y,0])
	children();
}

module next()
{
	if (n_cols > n_rows)
	{
		translate([0, n_rows*key_pitch+2*outer_radius+padding,0])
		children();
	}
	else
	{
		translate([n_cols*key_pitch+2*outer_radius+padding,0])
		children();
	}
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
