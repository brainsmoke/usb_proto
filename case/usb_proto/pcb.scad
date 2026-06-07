e=.0001;
b=1; // Big value which can be used in difference operations instead of e
     // to prevent render glitches.
$fn=48;

use <../lib/utils.scad>
use <../lib/usb.scad>

use <gen/components.scad>

hole_dist_x = 70;
hole_dist_y = 40;

pcb_radius = 3.5;
pcb_screw_hole_diameter = 3.2;
breadboard_hole_diameter = 1.;
breadboard_silkscreen_diameter = 2.2;
silkscreen_thickness = .1;


usb_c_board_offset=2.5 ;

button_pitch=4*2.54;
button_w=5;
button_d=5;
n_buttons=3;

function pcb_thickness()      = 1.6;
function pcb_center()         = [ hole_dist_x/2, hole_dist_y/2 ];
function dfu_button_pos()     = [ 7, 0 ];
function dfu_button_angle()   = 180;

function breadboard_pos(x, y) = [ 11 + 2.54*x, hole_dist_y/2 + (8.5-y)*2.54, 0 ];

function holes_pos()          = [ for (x = [0, hole_dist_x], y = [0, hole_dist_y]) [x,y] ];
function buttons_pos()        = [ for (i=[0:n_buttons-1]) [button_pitch*(i-(n_buttons-1)/2), -pcb_radius] ];
function button_height()      = 2;
function led_height()         = 1.2;
function led_arrays_pos()     = [ [ for(i=[0,1,2,3]) breadboard_pos(-1, 2+i) ] ];

components_origin = breadboard_pos(0,8.5);

module usb_c(margin=0)
{
	translate([0,-usb_c_board_offset,0]) 
	usb_c_type_c31_m12(margin)
	usb_c_plug(margin);
}

module usb_c_keepout()
{
	usb_c(margin=.5);
}

module pcb_components()
{
	translate(components_origin)
	{
		components_top();
 
		translate([0,0,-pcb_thickness()])
		components_bottom();
	}
}

module at_front()
{
	translate([-pcb_radius,hole_dist_y/2,0])
	rotate([0,0,90])
	children();
}

module at_back()
{
	translate([hole_dist_x+pcb_radius,hole_dist_y/2,0])
	rotate([0,0,-90])
	children();
}

module at_pcb_holes()
{
	for (p=holes_pos())
	translate(p)
	children();
}

module at_dfu_button()
{
	translate([ dfu_button_pos()[0], dfu_button_pos()[1], 0 ])
	rotate([0,0,dfu_button_angle()])
	children();
}

module at_buttons()
{
	at_back()
	for (p=buttons_pos())
	translate(p)
	children();
}

module for_each_led_array()
{
	for ($led_array=led_arrays_pos())
	children();
}

module at_leds()
{
	assert(is_list($led_array), "use: for_each_led_array() { ... at_leds() { ... } ... }");
	for (p=$led_array)
	translate( p )
	children();
}

module pcb_shape()
{
	hull()
	at_pcb_holes()
	translate([0,0,-pcb_thickness()])
	cylinder(pcb_thickness(), r=pcb_radius);
}

module pcb_pad()
{
	translate([0,0,-pcb_thickness()-b])
	cylinder(pcb_thickness()+2*b, r=breadboard_hole_diameter/2, $fn=8);
}

module pcb_pad_silk()
{
	translate([0,0,-pcb_thickness()-silkscreen_thickness])
	cylinder(pcb_thickness()+2*silkscreen_thickness, r=breadboard_silkscreen_diameter/2, $fn=8);
}

module at_pcb_single_pads()
{
	for (y=[2:15])
	translate(breadboard_pos(0,y))
	children();

	for (y=[4,5,8,9,12,13])
	translate(breadboard_pos(22,y))
	children();

	for (y=[8,9])
	translate(breadboard_pos(-2,y))
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

module pcb_holes_and_pads()
{
	translate([0,0,-pcb_thickness()-b])
	at_pcb_holes()
	cylinder(pcb_thickness()+2*b, r=pcb_screw_hole_diameter/2);

	at_pcb_pads()
	pcb_pad();
}

module pcb()
{
	color("green")
	render()
	difference()
	{
		pcb_shape();

		pcb_holes_and_pads();
	}


	color("white")
	render()
	difference()
	{
		union()
		{
			pcb_draw_breadboard_lines()
			pcb_pad_silk();

			at_pcb_single_pads()
			pcb_pad_silk();
		}

		pcb_holes_and_pads();
	}

	pcb_components();
}

module pcb_keepout()
{
	pcb_shape();

	at_front()
	usb_c_keepout();
}

preview()
pcb();

preview()
translate([0,60,0])
#pcb_keepout();
