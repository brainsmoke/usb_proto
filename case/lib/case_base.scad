e=.0001;
b=1; // Big value which can be used in difference operations instead of e
     // to prevent render glitches.
$fn=48;

use <graft.scad>
use <grid.scad>

inner_radius = 4;
wall_thickness = 1;
bottom_thickness = .8;
top_thickness = .8;
total_height = 16;
top_border_height = .8;
chamfer = 0;
leg_height = 4.6;

top_ledge_height = 2.5;
top_ledge_thickness = 1.;
top_ledge_margin = 0.;

leg_thickness = 1.2;

/* (countersunk) screw specs */
thread = 3;

head_diameter = 6;
head_thickness = 2.5;
screw_clearance = .25;
screw_min_grip = 3;
screw_min_guaranteed_depth = 10;

funnel_top_radius = 3.2;
funnel_top_height = 3;

case_split = bottom_thickness+leg_height;

chamfer_top = chamfer;
chamfer_bottom = chamfer;

chamfer_top_w = chamfer_top;
chamfer_top_h = chamfer_top;

chamfer_bottom_w = chamfer_bottom;
chamfer_bottom_h = chamfer_bottom;

outer_radius = inner_radius+wall_thickness;
screw_grab_radius = (thread*0.9)/2;
screw_loose_radius = (thread/0.9)/2;
funnel_top_inner_radius = funnel_top_radius-leg_thickness;

screw_guaranteed_depth = max(screw_min_guaranteed_depth, case_split + screw_min_grip);

grid_origin=[0,0];
grid_angle = 0;
grid_rows = undef;
grid_cols = undef;
grid_pitch = 10;
grid_width = .8;
grid_height_bottom = 1;
grid_height_top = 2;

chamfer_bottom_inner_h = max(0, chamfer_bottom_h-bottom_thickness);
chamfer_bottom_inner_w = chamfer_bottom_w/chamfer_bottom_h * chamfer_bottom_inner_h;

chamfer_top_inner_h = max(0, chamfer_top_h-top_thickness);
chamfer_top_inner_w = chamfer_top_w/chamfer_top_h * chamfer_top_inner_h;

assert((chamfer_top_h == 0) || (inner_radius-chamfer_top_inner_w > 0), "Chamfer too big");
assert((chamfer_bottom_h == 0) || (inner_radius-chamfer_bottom_inner_w > 0), "Chamfer too big");
assert(screw_guaranteed_depth <= total_height - top_thickness, "No room for screws");

module case_shape_convex(height, radius)
{
	hull()
	at_holes()
	cylinder(height, r=radius);
}

module case_shape_chamfer_convex(height, r1, r2)
{
	if (height != 0)
	translate([0,0,(r1 < r2) ? 0 : -e])
	hull()
	{
		case_shape(e, r1);
		translate([0,0,height])
		case_shape(e, r2);
	}
}

/* override with manual implementation for non-convex shapes */
module case_shape(height, radius)
{
	case_shape_convex(height, radius);
}

/* override with manual implementation for non-convex shapes */
module case_shape_chamfer(height, r1, r2)
{
	case_shape_chamfer_convex(height, r1, r2);
}

module screw_shape(padding=0, bottom_epsilon=0, top_epsilon=0)
{
	x = head_diameter/2 - screw_loose_radius;
	y = head_thickness;
	dz = (sqrt(x*x+y*y)/x - y/x)*padding;

	x2 = funnel_top_inner_radius - screw_grab_radius;
	y2 = funnel_top_height;
	dz2 = (sqrt(x*x+y*y)/x - y/x)*padding;

	z4 = screw_clearance+head_thickness+dz;
	z3 = max(z4, case_split-e/2);

	path = [
		[0,                               -bottom_epsilon],
		[padding+head_diameter/2,         -bottom_epsilon],
		[padding+head_diameter/2,         screw_clearance+dz],
		[padding+screw_loose_radius,      z4],
		[padding+screw_loose_radius,      z3],
		[padding+screw_grab_radius,       z3],
		[padding+screw_grab_radius,       total_height-top_thickness-funnel_top_height-dz2],

		[padding+funnel_top_inner_radius, total_height-top_thickness],
		[padding+funnel_top_inner_radius, total_height-top_thickness+top_epsilon],
		[0,                               total_height-top_thickness+top_epsilon],

 ];

	rotate_extrude() polygon(path);
}

module screw_guaranteed_cutout()
{
	intersection()
	{
		screw_shape(0, bottom_epsilon=b);

		translate([0,0,-e])
		cylinder(screw_guaranteed_depth+e, r=leg_thickness+head_diameter/2+1);
	}
}

module screw_opportunistic_cutout()
{
	intersection()
	{
		screw_shape(0, bottom_epsilon=b);
		union()
		{
			translate([0,0,screw_guaranteed_depth])
			cylinder(total_height-chamfer_top_h-screw_guaranteed_depth, r=inner_radius+e);

			translate([0,0,total_height-chamfer_top_h])
			cylinder(chamfer_top_inner_h, r1=inner_radius+e, r2=inner_radius-chamfer_top_inner_w);
		}
	}

}

module screw_guide(z_clamp_min, z_clamp_max)
{
	graft()
	{
		graft_base_add()
		intersection()
		{
			screw_shape(leg_thickness);
			translate([0,0,z_clamp_min])
			cylinder(z_clamp_max-z_clamp_min,r=max(screw_loose_radius,
			                                       funnel_top_inner_radius,
			                                       head_diameter/2) + leg_thickness + b);
		}

		graft_base_remove()
		screw_opportunistic_cutout();

		graft_remove()
		screw_guaranteed_cutout();
	}
}

module case_grid()
{
	grid(grid_origin, grid_angle, grid_pitch, grid_width, grid_rows, grid_cols)
	children();
}

module keepout() { }
module bottom_custom() { }
module top_custom() { }

module _bottom()
{
	graft()
	{
		graft_base_add()
		{
			difference()
			{
				case_shape(total_height-top_border_height-chamfer_top_h, outer_radius);

				translate([0,0,bottom_thickness])
				union()
				{
					translate([0,0,chamfer_bottom_inner_h])
					case_shape(total_height-top_border_height-chamfer_top_h-chamfer_bottom_inner_h, inner_radius);
					case_shape_chamfer(chamfer_bottom_inner_h, inner_radius-chamfer_bottom_inner_w, inner_radius);
				}
			}

			case_grid()
			translate([0,0,bottom_thickness-e])
			case_shape(grid_height_bottom+e, (outer_radius+inner_radius)/2);
		}

		graft_base_remove()
		difference()
		{
			case_shape(chamfer_bottom_h, outer_radius+b);
			case_shape_chamfer(chamfer_bottom_h, outer_radius-chamfer_bottom_w, outer_radius);
		}

		at_holes() screw_guide(bottom_thickness, case_split-e);

		graft_remove()
		keepout();

		bottom_custom();

		children();
	}
}

module _top()
{
	graft()
	{
		graft_base_add()
		{
			difference()
			{
				union()
				{
					translate([0,0,total_height-top_border_height-chamfer_top_h])
					case_shape(top_border_height+chamfer_top_h, outer_radius);
					translate([0,0,total_height-top_border_height-chamfer_top_h-top_ledge_height])
					case_shape(top_ledge_height+e, inner_radius);
				}

				intersection()
				{
					union()
					{
						translate([0,0,total_height-top_border_height-chamfer_top_h-top_ledge_height-b])
						case_shape(top_ledge_height+top_border_height+chamfer_top_h-top_thickness-chamfer_top_inner_h+b, inner_radius+e);
						translate([0,0,total_height-top_thickness-chamfer_top_inner_h])
						case_shape_chamfer(chamfer_top_inner_h, inner_radius, inner_radius-chamfer_top_inner_w);
					}
					translate([0,0,total_height-top_border_height-chamfer_top_h-top_ledge_height-e])
					case_shape(top_ledge_height+top_border_height+chamfer_top_h+e, inner_radius-top_ledge_thickness);
				}
			}

			case_grid()
			translate([0,0,total_height-top_thickness-grid_height_top])
			case_shape(grid_height_top+e, inner_radius-top_ledge_thickness/2);
		}

		graft_base_remove()
		difference()
		{
			translate([0,0,total_height-chamfer_top_h])
			case_shape(chamfer_top_h+e, outer_radius+b);
			translate([0,0,total_height-chamfer_top_h])
			case_shape_chamfer(chamfer_top_h, outer_radius, outer_radius-chamfer_top_w);
		}

		at_holes() screw_guide(case_split, total_height-e);

		graft_remove()
		keepout();

		top_custom();

		children();
	}
}

module bottom()
{
	render()
	graft()
	_bottom()
	children();
}

module top()
{
	render()
	graft()
	_top()
	children();
}

