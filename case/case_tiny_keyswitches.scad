
key_pitch = 19;
n_keys = 6;

include <case_tiny_base.scad>
use <graft.scad>

e=.001;

hole_dist_x = n_keys * key_pitch;

total_height=11;

has_dfu_button=false;

grid_rows=3;
grid_cols=n_keys*2+1;
grid_pitch=key_pitch/2;
grid_x_off=0;
grid_width=1.2;

hole_dim = 13.8;
panel_dim = key_pitch-1.2;
panel_thickness = 1.5;

module keyswitch()
{
	graft()
	{
		graft_add()
		{
			translate([-panel_dim/2, -panel_dim/2, -panel_thickness-e])
			cube([panel_dim, panel_dim, panel_thickness]);
		}
		graft_remove()
		{
			cube([hole_dim, hole_dim, 10], center=true);
			translate([-panel_dim/2, -panel_dim/2, -panel_thickness*2])
			cube([panel_dim, panel_dim, panel_thickness]);
		}
	}
}

preview()
{
	pcb();
	on_pcb()
	{
		at_front() usb_c();
	}
}

case();

next() flip() top()
{
	for (x = [ 1 : 2 : n_keys*2-1 ])
	for (y = [ 1 ])
	translate(grid_pos(x,y))
	keyswitch();
};

