e=.0001;
b=1; // Big value which can be used in difference operations instead of e
     // to prevent render glitches.
$fn=48;

use <../lib/utils.scad>
use <../lib/usb.scad>

use <gen/components.scad>

hole_dist_x = 30;
hole_dist_y = 20;

pcb_radius = 3.5;
pcb_screw_hole_diameter = 3.2;

usb_c_board_offset=2.5 ;

function pcb_thickness()      = .8;
function pcb_center()         = [ hole_dist_x/2, hole_dist_y/2 ];
function dfu_button_pos()     = [ 9, 0 ];
function dfu_button_angle()   = 180;

function holes_pos()          = [ for (x = [0, hole_dist_x], y = [0, hole_dist_y]) [x,y] ];
function buttons_pos()        = [ ];
function button_height()      = 2;
function led_height()         = 1.2;
function led_arrays_pos()     = [ ];

components_origin = [0, 10];

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

module pcb_mirror()
{
	children();

	translate([hole_dist_x,hole_dist_y,0])
	rotate([0,0,180])
	children();
}

module at_dfu_button()
{
	pcb_mirror()
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

module pcb_shape()
{
	hull()
	at_pcb_holes()
	translate([0,0,-pcb_thickness()])
	cylinder(pcb_thickness(), r=pcb_radius);
}

module pcb_holes_and_pads()
{
	at_pcb_holes()
	translate([0,0,-pcb_thickness()-b])
	cylinder(pcb_thickness()+2*b, r=pcb_screw_hole_diameter/2);
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

	pcb_components();
}

module pcb_keepout()
{
	pcb_shape();

	at_front()
	usb_c_keepout();

	at_back()
	usb_c_keepout();
}

preview()
pcb();

preview()
translate([0,60,0])
#pcb_keepout();
