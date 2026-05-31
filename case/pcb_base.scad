e=.0001;
b=1; // Big value which can be used in difference operations instead of e
     // to prevent render glitches.
$fn=48;

use <graft.scad>
use <usb.scad>
use <button.scad>

has_buttons=true;
has_dfu_button=true;
has_leds=true;

hole_dist_x = 70;
hole_dist_y = 40;

pcb_radius = 3.5;
pcb_thickness = 1.6;
pcb_screw_hole_diameter = 3.2;
breadboard_hole_diameter = 1.;
breadboard_silkscreen_diameter = 2.2;
silkscreen_thickness = .1;
button_height=2;
led_height = 1.2;

usb_c_board_offset=2.5 ;

button_pitch=4*2.54;
button_w=5;
button_d=5;
n_buttons=3;

button_depth = total_height-component_z-button_height;

dfu_button_pos = [ 7, 0 ];
dfu_button_angle = 180;

light_pipe_diameter = 2.2;
light_pipe_border = 1.2;
light_pipe_base_border = 3.2;
light_pipe_depth = total_height-component_z-led_height;
light_pipe_base_height = light_pipe_depth/3;

function breadboard_pos(x, y) = [ 11 + 2.54*x, hole_dist_y/2 + (8.5-y)*2.54, component_z ];

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

module pcb_shape()
{
	hull()
		at_pcb_holes()
			cylinder(pcb_thickness, r=pcb_radius);
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
	{
		translate([0,0,component_z-pcb_thickness])
		pcb_shape();

		on_pcb() at_front() usb_c_keepout();
	}
}

module case_button()
{
	button(button_w, button_d, top_thickness, button_depth);
}

module pcb_top_features()
{
	at_dfu_button() case_button();
	at_buttons() case_button();
	light_pipes();
}

module pcb_bottom_features()
{
}

