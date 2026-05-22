e=.0001;
b=1; // Big value which can be used in difference operations instead of e
     // to prevent render glitches.
$fn=48;
padding=5;

use <graft.scad>
use <usb.scad>
use <grid.scad>
use <button.scad>

has_buttons=true;
has_dfu_button=true;
has_leds=true;

hole_dist_x = 70;
hole_dist_y = 40;

inner_radius = 4;
pcb_radius = 3.5;
pcb_thickness = 1.6;
pcb_screw_hole_diameter = 3.2;
breadboard_hole_diameter = 1.;
breadboard_silkscreen_diameter = 2.2;
silkscreen_thickness = .1;
wall_thickness = 1;
bottom_thickness = .8;
top_thickness = .8;
total_height = 16;
button_height=2;
led_height = 1.2;

top_border_height = .8;

top_ledge_height = 2.5;
top_ledge_thickness = 1.;
top_ledge_margin = 0.;

chamfer = 0;

chamfer_top = chamfer;
chamfer_bottom = chamfer;

chamfer_top_w = chamfer_top;
chamfer_top_h = chamfer_top;

chamfer_bottom_w = chamfer_bottom;
chamfer_bottom_h = chamfer_bottom;

leg_height = 3;
leg_thickness = 1.2;

funnel_top_radius = 3.2;
funnel_top_height = 3;

usb_c_board_offset=2.5 ;

/* (countersunk) screw specs */
thread = 3;

head_diameter = 6;
head_thickness = 2.5;
screw_clearance = .25;
screw_min_grip = 3;
screw_min_guaranteed_depth = 10;

button_pitch=4*2.54;
button_w=5;
button_d=5;
n_buttons=3;

outer_radius = inner_radius+wall_thickness;
screw_grab_radius = (thread*0.9)/2;
screw_loose_radius = (thread/0.9)/2;
funnel_top_inner_radius = funnel_top_radius-leg_thickness;
component_z = bottom_thickness+leg_height+pcb_thickness;

screw_guaranteed_depth = max(screw_min_guaranteed_depth, component_z + screw_min_grip);

button_depth = total_height-component_z-button_height;

bounding_box = [ [            -outer_radius,            -outer_radius, 0           ],
                 [ hole_dist_x+outer_radius, hole_dist_y+outer_radius, total_height] ];

dfu_button_pos = [ 7, 0 ];
dfu_button_angle = 180;

grid_dim = bounding_box;
grid_x_off = 0;
grid_y_off = 0;
grid_rows = 4;
grid_cols = 6;
grid_pitch = 10;
grid_width = .8;
grid_height_bottom = 1;
grid_height_top = 2;

light_pipe_diameter = 2.2;
light_pipe_border = 1.2;
light_pipe_base_border = 3.2;
light_pipe_depth = total_height-component_z-led_height;
light_pipe_base_height = light_pipe_depth/3;

assert(screw_guaranteed_depth <= total_height - top_thickness);

function breadboard_pos(x, y) = [ 11 + 2.54*x, hole_dist_y/2 + (8.5-y)*2.54, component_z ];

module preview()
{
	if ($preview) children();
}

module on_pcb()
{
	translate([0,0,component_z]) children();
}

module usb_c(margin=0)
{
	translate([0,-usb_c_board_offset,0]) 
	usb_c_type_c31_m12(margin) usb_c_plug(margin);
}

module usb_c_keepout()
{
	graft()
	graft_remove()
	usb_c(margin=.5);
}

module at_front()
{
	translate([-pcb_radius,hole_dist_y/2,0]) rotate([0,0,90])  children();
}

module at_back()
{
	translate([hole_dist_x+pcb_radius,hole_dist_y/2,0]) rotate([0,0,-90])  children();
}

module at_pcb_holes()
{
	for (x = [0, hole_dist_x])
		for (y = [0, hole_dist_y])
			translate([x,y,0])
				children();
}

module at_extra_holes()
{
}

module at_holes()
{
	at_pcb_holes() children();
	at_extra_holes() children();
}

module at_dfu_button()
{
	if (has_dfu_button)
		translate([ dfu_button_pos[0], dfu_button_pos[1], total_height ])
			rotate([0,0,dfu_button_angle])
				children();
}

module at_buttons()
{
	if (has_buttons)
		at_back()
			for (i=[0:n_buttons-1])
				translate([button_pitch*(i-(n_buttons-1)/2), -pcb_radius, total_height])
					children();
}

module at_light_pipes()
{
	if (has_leds)
	for (i=[0,1,2,3])
		translate( breadboard_pos(-1, 2+i) )
		translate( [0,0,led_height] )
			children();
}

module light_pipes()
{
	graft()
	{
		graft_add()
		hull()
		at_light_pipes()
		cylinder(light_pipe_depth, r=light_pipe_diameter/2+light_pipe_border);

		graft_add()
		hull()
		at_light_pipes()
		translate([0,0,light_pipe_depth-light_pipe_base_height])
		cylinder(light_pipe_base_height,
			r1=light_pipe_diameter/2+light_pipe_border,
			r2=light_pipe_diameter/2+light_pipe_base_border);

		graft_remove()
		at_light_pipes()
		translate([0,0,-b])
		cylinder(light_pipe_depth+2*b, r=light_pipe_diameter/2);

	}
}

module case_shape(height, radius)
{
	hull()
		at_holes()
			cylinder(height, r=radius);
}

module case_shape_chamfer(height, r1, r2)
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

module pcb_shape()
{
	hull()
		at_pcb_holes()
			cylinder(pcb_thickness, r=pcb_radius);
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
	z3 = max(z4, bottom_thickness+leg_height+pcb_thickness-e/2);

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
		translate([0,0,-e]) cylinder(screw_guaranteed_depth+e, r=leg_thickness+head_diameter/2+1);
	}
}

module screw_opportunistic_cutout()
{
	difference()
	{
		screw_shape(0, bottom_epsilon=b);
		translate([0,0,-b-e]) cylinder(screw_guaranteed_depth+b+e, r=leg_thickness+head_diameter/2+1);
	}
}

module leg()
{
	graft()
	{
		graft_add()
		difference()
		{
			intersection()
			{
				screw_shape(leg_thickness);
				translate([0,0,bottom_thickness])
				cylinder(component_z-e-bottom_thickness, r=leg_thickness+head_diameter/2+1);
			}
			screw_opportunistic_cutout();
		}

		graft_remove()
		screw_guaranteed_cutout();
	}
}

module screw_guide()
{
	graft()
	{
		graft_add()
		difference()
		{
			intersection()
			{
				screw_shape(leg_thickness);
				translate([0,0,component_z])
				cylinder(total_height-component_z-e, r=max(screw_loose_radius,funnel_top_inner_radius)+leg_thickness+1);
			}
			screw_opportunistic_cutout();
		}

		graft_remove()
		screw_guaranteed_cutout();
	}
}

module pcb_pad()
{
	translate([0,0,-pcb_thickness-b])
	cylinder(pcb_thickness+2*b, r=breadboard_hole_diameter/2, $fn=8);
}

module pcb_pad_silk()
{
	translate([0,0,-pcb_thickness-silkscreen_thickness])
	cylinder(pcb_thickness+2*silkscreen_thickness, r=breadboard_silkscreen_diameter/2, $fn=8);
}

module at_pcb_single_pads()
{
	for (y=[2:15])
	translate(breadboard_pos(0,y))
	children();

	for (y=[4,5,8,9,12,13])
	translate(breadboard_pos(22,y))
	children();
}

module at_pcb_pads()
{
	at_pcb_single_pads()
	children();

	for (x=[1:21])
	for (y=[0:17])
	translate(breadboard_pos(x,y))
	children();
}

module pcb_draw_breadboard_lines()
{
	for (y=[0, 1, 16, 17])
	hull()
	{
		translate(breadboard_pos(1,y))
		children();

		translate(breadboard_pos(21,y))
		children();
	}

	for (y=[2, 9])
	for (x=[1:21])
	hull()
	{
		translate(breadboard_pos(x,y))
		children();

		translate(breadboard_pos(x,y+6))
		children();

	}
}

module pcb()
{
	difference()
	{
		union()
		{
			color("green")
			translate([0,0,component_z-pcb_thickness])
			pcb_shape();

			color("white")
			{
				pcb_draw_breadboard_lines()
				pcb_pad_silk();

				at_pcb_single_pads()
				pcb_pad_silk();
			}
		}

		union()
		{
			translate([0,0,component_z-pcb_thickness-b])
			at_pcb_holes()
			cylinder(pcb_thickness+2*b, r=pcb_screw_hole_diameter/2);

			at_pcb_pads()
			pcb_pad();
		}
	}
}

module pcb_keepout()
{
	graft()
	graft_remove()
	translate([0,0,component_z-pcb_thickness])
	pcb_shape();
}

module bottom_extra() { }

module case_grid(thickness)
{
	x=grid_dim[0][0];
	y=grid_dim[0][1];
	w=grid_dim[1][0]-grid_dim[0][0];
	d=grid_dim[1][1]-grid_dim[0][1];
	translate([x,y,0])
	grid(w,d, thickness, grid_rows, grid_cols, grid_pitch, bar_width=grid_width, x_off=grid_x_off, y_off=grid_y_off);
}

module bottom()
{
	render()
	graft()
	{
		at_holes() leg();

		graft_base_add()
		{
			difference()
			{
				union()
				{
					translate([0,0,chamfer_bottom_h])
					case_shape(total_height-top_border_height-chamfer_bottom_h, outer_radius);
					case_shape_chamfer(chamfer_bottom_h, outer_radius-chamfer_bottom_w, outer_radius);
				}
				translate([0,0,bottom_thickness])
				union()
				{
					chamfer_inner_h = max(0, chamfer_bottom_h-bottom_thickness);
					chamfer_inner_w = chamfer_bottom_w/chamfer_bottom_h * chamfer_inner_h;
					translate([0,0,chamfer_inner_h])
					case_shape(total_height-top_border_height-chamfer_inner_h, inner_radius);
					case_shape_chamfer(chamfer_inner_h, inner_radius-chamfer_inner_w, inner_radius);
				}
			}
			intersection()
			{
				translate([0,0,e])
				union()
				{
					translate([0,0,chamfer_bottom_h])
					case_shape(total_height-top_border_height-chamfer_bottom_h, outer_radius);
					case_shape_chamfer(chamfer_bottom_h, outer_radius-chamfer_bottom_w, outer_radius);
				}

				translate([0,0,bottom_thickness-e])
				case_grid(grid_height_bottom+e);
			}
		}
		on_pcb() at_front() usb_c_keepout();

		bottom_extra();

		pcb_keepout();

		children();
	}
}

module top_extra() { }

module top()
{
	render()
	graft()
	{
		graft_base_add()
		{
			difference()
			{
				union()
				{
					translate([0,0,total_height-top_border_height])
					case_shape(top_border_height-chamfer_top_h, outer_radius);
					translate([0,0,total_height-chamfer_top_h])
					case_shape_chamfer(chamfer_top_h, outer_radius, outer_radius-chamfer_top_w);
					translate([0,0,total_height-top_border_height-top_ledge_height])
					case_shape(top_ledge_height+e, inner_radius);
				}

				intersection()
				{
					union()
					{
						chamfer_inner_h = max(0, chamfer_top_h-top_thickness);
						chamfer_inner_w = chamfer_top_w/chamfer_top_h * chamfer_inner_h;

						translate([0,0,total_height-top_border_height-top_ledge_height-b])
						case_shape(top_ledge_height+top_border_height-top_thickness-chamfer_inner_h+b, inner_radius+e);
						translate([0,0,total_height-top_thickness-chamfer_inner_h])
						case_shape_chamfer(chamfer_inner_h, inner_radius, inner_radius-chamfer_inner_w);
					}
					translate([0,0,total_height-top_border_height-top_ledge_height-e])
					case_shape(top_ledge_height+top_border_height+e, inner_radius-top_ledge_thickness);
				}
			}

			intersection()
			{
				translate([0,0,-e])
				case_shape(total_height+2*e, inner_radius-top_ledge_thickness/2);

				translate([0,0,total_height-top_thickness-grid_height_top])
				case_grid(grid_height_top+e);
			}
		}

		at_holes() screw_guide();

		top_extra();

		pcb_keepout();

		children();
	}
}

module case_button()
{
	button(button_w, button_d, top_thickness, button_depth);
}

module top_features()
{
	at_dfu_button() case_button();
	at_buttons() case_button();
	light_pipes();
}

module flip()
{
	translate([ 0, hole_dist_y, total_height]) rotate([180,0,0]) children();
}

module next()
{
	translate([ 0, hole_dist_y+2*outer_radius+padding, 0 ]) children();
}
