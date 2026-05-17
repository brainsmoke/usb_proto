
key_pitch = 19;

include <case_base.scad>
use <keyswitch.scad>

grid_rows=5;
grid_cols=8;
grid_pitch=key_pitch/2;
grid_x_off=2;
grid_width=1.2;

panel_dim = key_pitch-grid_width;

flip() top()
{
	light_pipes();
	at_dfu_button() case_button();

	for (x = [ 2, 4, 6 ])
	for (y = [ 1, 3 ])
	translate(grid_pos(x,y))
	keyswitch_panel(panel_dim);
};

