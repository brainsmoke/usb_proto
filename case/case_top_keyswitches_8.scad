
key_pitch = 19;

include <case_base.scad>
use <keyswitch.scad>

grid_rows=5;
grid_cols=9;
grid_pitch=key_pitch/2;
grid_x_off=0;
grid_width=1.2;

panel_dim = key_pitch-grid_width;
panel_thickness = 1.5;

flip() top()
{
	for (x = [ 1, 3, 5, 7 ])
	for (y = [ 1, 3 ])
	translate(grid_pos(x,y))
	keyswitch_panel(panel_dim, panel_thickness);
};

