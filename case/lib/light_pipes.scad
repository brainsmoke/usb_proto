
use <graft.scad>

b=1;
light_pipe_diameter = 2.2;
light_pipe_border = 1.2;
light_pipe_base_border = 3.2;
light_pipe_depth = 12;
light_pipe_base_height = light_pipe_depth/3;

module _draw_only(s)
{
	if (is_undef($_stage) || $_stage == s)
	{
		$_stage = s;
		children();
	}
}

module light_pipe_hull()
{
	hull()
	_draw_only("tube")
	children();

	hull()
	_draw_only("base")
	children();

	_draw_only("hole")
	children();
}

module light_pipe(depth       =light_pipe_depth,
                  diameter    =light_pipe_diameter,
                  border      =light_pipe_border,
                  base_border =light_pipe_base_border,
                  base_height =light_pipe_base_height)
{
	translate([0,0,-depth])
	{
		_draw_only("tube")
		graft_add()
		cylinder(depth, r=diameter/2+border);

		_draw_only("base")
		graft_add()
		translate([0,0,depth-base_height])
		cylinder(base_height, r1=diameter/2+border, r2=diameter/2+base_border);

		_draw_only("hole")
		graft_remove()
		translate([0,0,-b])
		cylinder(depth+2*b, r=diameter/2);
	}
}

