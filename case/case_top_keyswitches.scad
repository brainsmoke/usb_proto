
include <case_base.scad>
use <sculpt.scad>

e=.001;

grid_rows=5;
grid_cols=8;
grid_pitch=19/2;
grid_x_off=2;
grid_width=1.2;

hole_dim = 13.8;
panel_dim = 19-1.2;
panel_thickness = 1.5;

module keyswitch()
{
	sculpt()
	{
		sculpt_add()
		{
			translate([-panel_dim/2, -panel_dim/2, -panel_thickness-e])
			cube([panel_dim, panel_dim, panel_thickness]);
		}
		sculpt_carve()
		{
			cube([hole_dim, hole_dim, 10], center=true);
			translate([-panel_dim/2, -panel_dim/2, -panel_thickness*2])
			cube([panel_dim, panel_dim, panel_thickness]);
		}
	}
}

flip() top()
{
	light_pipes();
	at_dfu_button() case_button();

	for (x = [ 2, 4, 6 ])
	for (y = [ 1, 3 ])
	translate(grid_pos(x,y))
	keyswitch();
};

