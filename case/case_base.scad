e=.0001;
$fn=48;
padding=5;

use <usb.scad>
use <grid.scad>
use <button.scad>

with_buttons=false;
with_dfu_button=true;

hole_dist_x = 70;
hole_dist_y = 40;

inner_radius = 4;
pcb_radius = 3.5;
pcb_thickness = 1.6;
wall_thickness = 1;
bottom_thickness = .8;
top_thickness = .8;
total_height = 16;
button_height=2;

top_edge_height = 2.5;
top_edge_thickness = 1.;
top_edge_margin = 0.;

leg_height = 3;
leg_thickness = 1;

funnel_top_radius = 3;
funnel_top_height = 3;

usb_c_board_offset=2.5 ;

/* (countersunk) screw specs */
thread = 3;
head_diameter = 6;
head_thickness = 2.5;
screw_clearance = .25;

button_pitch=4*2.54;
button_w=5;
button_d=5;
n_buttons=3;

outer_radius = inner_radius+wall_thickness;
screw_grab_radius = (thread*0.9)/2;
screw_loose_radius = (thread/0.9)/2;
funnel_top_inner_radius = funnel_top_radius-leg_thickness;
component_z = bottom_thickness+leg_height+pcb_thickness;

button_depth = total_height-component_z-button_height;

dfu_button_pos = [ 63, 40 ];
dfu_button_angle = 0;

grid_rows = 4;
grid_cols = 6;
grid_pitch = 10;
grid_width = .8;
grid_height_bottom = 1;
grid_height_top = 2;



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
	usb_c(margin=.5);
}

module at_front()
{
	translate([hole_dist_x+pcb_radius,hole_dist_y/2,0]) rotate([0,0,-90])  children();
}

module at_back()
{
	translate([-pcb_radius,hole_dist_y/2,0]) rotate([0,0,90])  children();
}

module at_top()
{
	translate([hole_dist_x/2,hole_dist_y+pcb_radius,0]) children();
}

module at_holes(x, y)
{
	for (x = [0, hole_dist_x])
		for (y = [0, hole_dist_y])
			translate([x,y,0])
				children();
}

module at_dfu_button()
{
	if (with_dfu_button)
		translate([ dfu_button_pos[0], dfu_button_pos[1], total_height ])
			rotate([0,0,dfu_button_angle])
				children();
}

module at_buttons()
{
	if (with_buttons)
	{
		at_back()
			for (i=[0:n_buttons-1])
				translate([button_pitch*(i-(n_buttons-1)/2), -pcb_radius, total_height])
					children();

		at_dfu_button() children();
	}
}

module case_shape(height, radius)
{
	hull()
		at_holes()
			cylinder(height, r=radius);
}

module screw_shape(padding=0, bottom_epsilon=0, top_epsilon=0)
{
	x = head_diameter/2 - screw_loose_radius;
	y = head_thickness;
	dz = (sqrt(x*x+y*y)/x - y/x)*padding;

	x2 = funnel_top_inner_radius - screw_grab_radius;
	y2 = funnel_top_height;
	dz2 = (sqrt(x*x+y*y)/x - y/x)*padding;

	path = [
		[0, -e],
		[padding+head_diameter/2, -e],
		[padding+head_diameter/2, screw_clearance+dz],
		[padding+screw_loose_radius, screw_clearance+head_thickness+dz],
		[padding+screw_loose_radius, bottom_thickness+leg_height+e],
		[padding+screw_grab_radius, bottom_thickness+leg_height+e],
		[padding+screw_grab_radius, total_height-top_thickness-funnel_top_height-dz2],
		[padding+funnel_top_inner_radius, total_height-top_thickness+e],
		[0, total_height-top_thickness+e],

 ];

	rotate_extrude() polygon(path);
}

module leg()
{
	intersection()
	{
		screw_shape(leg_thickness);
		translate([0,0,e]) cylinder(leg_height-e, r=leg_thickness+head_diameter/2+1); 
	}
}

module leg_keepout()
{
	screw_shape(0, bottom_epsilon=-e);
}

module pcb()
{
	translate([0,0,bottom_thickness+leg_height])
		color("green")
			case_shape(pcb_thickness, pcb_radius);
}

module keepout_zones()
{
	at_holes() leg_keepout();
	on_pcb() at_front() usb_c_keepout();
	at_buttons() button_keepout(button_w, button_d, top_thickness, button_depth);

}

module case()
{
	difference()
	{
		union()
		{
			at_holes() translate([0, 0, bottom_thickness]) leg();
			difference()
			{
				case_shape(total_height-top_thickness, outer_radius);
				translate([0,0,bottom_thickness])
				case_shape(total_height-top_thickness, inner_radius);
			}
			width = 2*outer_radius + hole_dist_x;
			depth = 2*outer_radius + hole_dist_y;

			translate([-outer_radius+e,-outer_radius+e,bottom_thickness-e])
			grid(width-e*2, depth-e*2, grid_height_bottom+e, grid_rows, grid_cols, grid_pitch, bar_width=grid_width);
		}
		keepout_zones();
	}
}

module top()
{
	at_buttons() button_shape(button_w, button_d, top_thickness, button_depth);
	difference()
	{
		union()
		{

			difference()
			{
				translate([0,0,total_height-top_thickness])
				{
					case_shape(top_thickness, outer_radius);
					translate([0,0,-top_edge_height])
						case_shape(top_edge_height+e, inner_radius);
				}
				translate([0,0,total_height-top_thickness-top_edge_height-e])
					case_shape(top_edge_height+e, inner_radius-top_edge_thickness);
			}

			at_holes() intersection()
			{
				screw_shape(leg_thickness);
				translate([0,0,component_z])
				cylinder(total_height-component_z-e, r=max(screw_loose_radius,funnel_top_inner_radius)+leg_thickness+1);
			}

			width = 2*inner_radius + hole_dist_x;
			depth = 2*inner_radius + hole_dist_y;

			translate([-inner_radius+e,-inner_radius+e,total_height-top_thickness-grid_height_top])
			grid(width-e*2, depth-e*2, grid_height_top+e, grid_rows, grid_cols, grid_pitch, bar_width=grid_width);


		}
		keepout_zones();
	}

}

module flip()
{
	translate([ 0, hole_dist_y, total_height]) rotate([180,0,0]) children();
}

module next()
{
	translate([ 0, hole_dist_y+2*outer_radius+padding, 0 ]) children();
}

