
include <case_base.scad>
use <graft.scad>

e=.001;

grid_rows=5;
grid_cols=9;
grid_pitch=19/2;
grid_x_off=0;
grid_width=1.2;

hole_dim = 13.8;
panel_dim = 19-1.2;
panel_thickness = 1.5;

module keyswitch()
{
	graft()
	{
		graft_add()
		{
			translate([-panel_dim/2, -panel_dim/2, -panel_thickness])
			cube([panel_dim, panel_dim, panel_thickness]);
		}
		graft_remove()
		{
			cube([hole_dim, hole_dim, 10], center=true);
			translate([-panel_dim/2-e, -panel_dim/2-e, -panel_thickness*2])
			cube([panel_dim+2*e, panel_dim+2*e, panel_thickness+e]);
		}
	}
}

flip() top()
{
	for (x = [ 1, 3, 5, 7 ])
	for (y = [ 1, 3 ])
	translate(grid_pos(x,y))
	keyswitch();
};

