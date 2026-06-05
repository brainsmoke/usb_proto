
key_pitch = 20;
n_keys = 4;

include <case_pcb.scad>
use <../lib/keyswitch.scad>

hole_dist_x = n_keys * key_pitch;

total_height=19;

/*
chamfer_top=4.35;
chamfer_top_h=9;
chamfer_bottom=.6;
*/

has_dfu_button=false;

grid_pitch=key_pitch/2;
grid_width=.8;

panel_dim = key_pitch-0.8;
panel_thickness = 1.5;

grid_height_bottom=0.8;

module at_keys()
{
	for (x = [ 0 : n_keys-1 ])
	for (y = [ 0 ])
	translate([x*key_pitch+key_pitch/2,y*key_pitch+key_pitch/2,total_height])
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
