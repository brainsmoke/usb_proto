
key_pitch = 19;

include <case_base.scad>
use <keyswitch.scad>

grid_rows=5;
grid_cols=9;
grid_pitch=key_pitch/2;
grid_x_off=0;
grid_width=1.2;

panel_dim = key_pitch-grid_width;

n_cols=4;
n_rows=2;

flip() top()
{
	translate([hole_dist_x/2,hole_dist_y/2,total_height])
    translate([-(n_cols)*key_pitch/2,-(n_rows)*key_pitch/2,0])
    for (x = [ 0 : n_cols-1 ])
    for (y = [ 0 : n_rows-1 ])
    translate([x*key_pitch+key_pitch/2,y*key_pitch+key_pitch/2,0])
	keyswitch_panel(panel_dim);
};

