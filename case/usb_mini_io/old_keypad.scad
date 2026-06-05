
key_pitch = 20;
n_keys = 4;

include <case_pcb.scad>

use <../lib/keyswitch.scad>

hole_dist_x = n_keys * key_pitch;

total_height=12.5;

has_dfu_button=false;

grid_pitch=key_pitch/2;
grid_width=1.2;

panel_dim = key_pitch-grid_width;

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
	keyswitch_panel(panel_dim);
}

module mockup()
{
	preview()
	{
		on_pcb()
		pcb();

		at_keys()
		at_mx_panel()
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
