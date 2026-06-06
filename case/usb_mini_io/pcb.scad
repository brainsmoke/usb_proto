e=.0001;
b=1; // Big value which can be used in difference operations instead of e
     // to prevent render glitches.
$fn=48;

use <../lib/utils.scad>
use <../lib/usb.scad>

use <gen/components.scad>

hole_dist_y = 20;

pcb_radius = 3.5;
pcb_depth = 22;
pcb_radius_back = 2;

pcb_screw_hole_diameter = 3.2;
breadboard_hole_diameter = 1.;
breadboard_silkscreen_diameter = 2.2;
silkscreen_thickness = .1;

components_origin = [15,10,0];

usb_c_board_offset=2.5 ;

button_pitch=4*2.54;
button_w=5;
button_d=5;
n_buttons=3;

function pcb_thickness()      = 1.;
function pcb_center()         = [ 7.5, hole_dist_y/2 ];
function dfu_button_pos()     = [ 7, 0 ];
function dfu_button_angle()   = 180;

function breadboard_pos(x, y) = [ 15 + 2.54*x, hole_dist_y/2 + (4.5-y)*2.54, 0 ];

function holes_pos()          = [ for (x = [0], y = [0, hole_dist_y]) [x,y] ];
function buttons_pos()        = [ ];
function button_height()      = 2;
function led_height()         = 1.2;
function led_arrays_pos()     = [ ];

module usb_c(margin=0)
{
	translate([0,-usb_c_board_offset,0])
	usb_c_type_c31_m12(margin)
	usb_c_plug(margin);
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

module usb_c_keepout()
{
	usb_c(margin=.5);
}

module at_front()
{
	translate([-pcb_radius,hole_dist_y/2,0])
	rotate([0,0,90])
	children();
}

module at_back()
{
	translate([pcb_depth-pcb_radius,hole_dist_y/2,0])
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

module pcb_shape()
{
	hull()
	translate([0,0,-pcb_thickness()])
	{
		at_pcb_holes()
		cylinder(pcb_thickness(), r=pcb_radius);

		for (x = [pcb_depth-pcb_radius-pcb_radius_back])
		for (y = [pcb_radius_back-pcb_radius, hole_dist_y+pcb_radius-pcb_radius_back])
		translate([x,y,0])
		cylinder(pcb_thickness(), r=pcb_radius_back);
	}
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

module at_pcb_pads()
{
	for (y=[0:9])
	translate(breadboard_pos(0, y))
	children();

	for (y=[6:9])
	translate(breadboard_pos(-1, y))
	children();
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
		at_pcb_pads()
		pcb_pad_silk();

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
