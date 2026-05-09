
key_pitch = 19;
n_keys = 4;

include <case_tiny_base.scad>
use <keyswitch.scad>

hole_dist_x = n_keys * key_pitch;

total_height=12.5;

has_dfu_button=false;

grid_rows=3;
grid_cols=n_keys*2+1;
grid_pitch=key_pitch/2;
grid_x_off=0;
grid_width=1.2;

panel_dim = key_pitch-grid_width;
panel_thickness = 1.5;

module at_keys()
{
	for (x = [ 1 : 2 : n_keys*2-1 ])
	for (y = [ 1 ])
	translate(grid_pos(x,y))
	children();
}

module top_features()
{
	at_keys()
	keyswitch_panel(panel_dim, panel_thickness);
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
